/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	This implements a cryptographically secure pseudorandom number generator for implementing secure demos. The approach taken is to use the
	AES (Rijndael) stream cipher in "counter" mode, encrypting an incrementing counter. The cipher key acts as the random seed. Cryptanalysis
	of AES used in this way has shown it to be an effective PRNG (see: Empirical Evidence concerning AES, Hellekalek & Wegenkittl, 2003).

	AES implementation taken from Linux kernel's AES implementation, found in crypto/aes_generic.c. It has been hacked to work independently.
\**********************************************************************************************************************************************/
/**********************************************************************************************************************************************\
 * Cryptographic API.
 * AES Cipher Algorithm.
 * Based on Brian Gladman's code.
 *
 * Linux developers:
 * Alexander Kjeldaas <astor@fast.no>
 * Herbert Valerio Riedel <hvr@hvrlab.org>
 * Kyle McMartin <kyle@debian.org>
 * Adam J. Richter <adam@yggdrasil.com> (conversion to 2.5 API).
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 * ---------------------------------------------------------------------------
 * Copyright (c) 2002, Dr Brian Gladman <brg@gladman.me.uk>, Worcester, UK.
 * All rights reserved.
 *
 * LICENSE TERMS
 *	The free distribution and use of this software in both source and binary form is allowed (with or without changes) provided that:
 *	1. distributions of this source code include the above copyright notice, this list of conditions and the following disclaimer;
 *	2. distributions in binary form include the above copyright notice, this list of conditions and the following disclaimer in the
 *		documentation and/or other associated materials;
 *	3. the copyright holder's name is not used to endorse products built using this software without specific written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this product may be distributed under the terms of the
 *	GNU General Public License (GPL), in which case the provisions of the GPL apply INSTEAD OF those given above.
 *
 * DISCLAIMER
 *	This software is provided 'as is' with no explicit or implied warranties in respect of its properties, including, but not limited to,
 *	correctness and/or fitness for purpose.
\**********************************************************************************************************************************************/

#include "aes_prng.h"
#include "aes_prng_tables.h"

#include "i_swap.h"

constexpr size_t AES_MIN_KEY_SIZE{16};
constexpr size_t AES_MAX_KEY_SIZE{32};
constexpr size_t AES_KEYSIZE_128{16};
constexpr size_t AES_KEYSIZE_192{24};
constexpr size_t AES_KEYSIZE_256{32};
constexpr size_t AES_BLOCK_SIZE{16};
constexpr size_t AES_MAX_KEYLENGTH{(15 * 16)};
constexpr size_t AES_MAX_KEYLENGTH_U32{(AES_MAX_KEYLENGTH / sizeof(uint32_t))};

// Please ensure that the first two fields are 16-byte aligned relative to the start of the structure, i.e., don't move them!
struct aes_context_t
{
	uint32_t key_enc[AES_MAX_KEYLENGTH_U32];
	uint32_t key_dec[AES_MAX_KEYLENGTH_U32];
	uint32_t key_length;
};

static inline uint8_t get_byte(const uint32_t x, const unsigned n)
{
	return x >> (n << 3);
}

// initialize the key schedule from the user supplied key
static inline uint32_t aes_ror32(const uint32_t word, const unsigned shift)
{
	return (word >> shift) | (word << (32 - shift));
}

static inline uint32_t star_x(const uint32_t word)
{
	return ((word & 0x7f7f7f7f) << 1) ^ (((word & 0x80808080) >> 7) * 0x1b);
}

#define cpu_to_le32(x) SDL_SwapLE32(x)
#define le32_to_cpu(x) SDL_SwapLE32(x)

#define ls_box(x)								\
	crypto_fl_tab[0][get_byte(x, 0)] ^			\
	crypto_fl_tab[1][get_byte(x, 1)] ^			\
	crypto_fl_tab[2][get_byte(x, 2)] ^			\
	crypto_fl_tab[3][get_byte(x, 3)]

#define loop4(i)	do {						\
	t = aes_ror32(t, 8);						\
	t = ls_box(t) ^ rco_tab[i];					\
	t ^= ctx.key_enc[4 * i];					\
	ctx.key_enc[4 * i + 4] = t;					\
	t ^= ctx.key_enc[4 * i + 1];				\
	ctx.key_enc[4 * i + 5] = t;					\
	t ^= ctx.key_enc[4 * i + 2];				\
	ctx.key_enc[4 * i + 6] = t;					\
	t ^= ctx.key_enc[4 * i + 3];				\
	ctx.key_enc[4 * i + 7] = t;					\
} while (0)

#define loop6(i)	do {						\
	t = aes_ror32(t, 8);						\
	t = ls_box(t) ^ rco_tab[i];					\
	t ^= ctx.key_enc[6 * i];					\
	ctx.key_enc[6 * i + 6] = t;					\
	t ^= ctx.key_enc[6 * i + 1];				\
	ctx.key_enc[6 * i + 7] = t;					\
	t ^= ctx.key_enc[6 * i + 2];				\
	ctx.key_enc[6 * i + 8] = t;					\
	t ^= ctx.key_enc[6 * i + 3];				\
	ctx.key_enc[6 * i + 9] = t;					\
	t ^= ctx.key_enc[6 * i + 4];				\
	ctx.key_enc[6 * i + 10] = t;				\
	t ^= ctx.key_enc[6 * i + 5];				\
	ctx.key_enc[6 * i + 11] = t;				\
} while (0)

#define loop8tophalf(i) do {					\
	t = aes_ror32(t, 8);						\
	t = ls_box(t) ^ rco_tab[i];					\
	t ^= ctx.key_enc[8 * i];					\
	ctx.key_enc[8 * i + 8] = t;					\
	t ^= ctx.key_enc[8 * i + 1];				\
	ctx.key_enc[8 * i + 9] = t;					\
	t ^= ctx.key_enc[8 * i + 2];				\
	ctx.key_enc[8 * i + 10] = t;				\
	t ^= ctx.key_enc[8 * i + 3];				\
	ctx.key_enc[8 * i + 11] = t;				\
} while (0)

#define loop8(i)	do {						\
	loop8tophalf(i);							\
	t = ctx.key_enc[8 * i + 4] ^ ls_box(t);		\
	ctx.key_enc[8 * i + 12] = t;				\
	t ^= ctx.key_enc[8 * i + 5];				\
	ctx.key_enc[8 * i + 13] = t;				\
	t ^= ctx.key_enc[8 * i + 6];				\
	ctx.key_enc[8 * i + 14] = t;				\
	t ^= ctx.key_enc[8 * i + 7];				\
	ctx.key_enc[8 * i + 15] = t;				\
} while (0)

/**
 * AES_ExpandKey - Expands the AES key as described in FIPS-197
 * @ctx:	The location where the computed key will be stored.
 * @in_key:		The supplied key.
 * @key_len:	The length of the supplied key.

	Returns 0 on success. The function fails only if an invalid key size (or pointer) is supplied. The expanded key size is 240 bytes (max of 14
	rounds with a unique 16 bytes key schedule plus a 16 bytes key which is used before the first round). The decryption key is prepared for the
	"Equivalent Inverse Cipher" as described in FIPS-197. The first slot (16 bytes) of each key (enc or dec) is for the initial combination, the
	second slot for the first round and so on.
 */
static int AES_ExpandKey(aes_context_t& ctx, const uint8_t* in_key, const unsigned key_len)
{
	const uint32_t* key{(const uint32_t*)in_key};

	if (key_len != AES_KEYSIZE_128 && key_len != AES_KEYSIZE_192 && key_len != AES_KEYSIZE_256)
	{
		return -1;
	}

	ctx.key_length = key_len;

	ctx.key_dec[key_len + 24] = ctx.key_enc[0] = le32_to_cpu(key[0]);
	ctx.key_dec[key_len + 25] = ctx.key_enc[1] = le32_to_cpu(key[1]);
	ctx.key_dec[key_len + 26] = ctx.key_enc[2] = le32_to_cpu(key[2]);
	ctx.key_dec[key_len + 27] = ctx.key_enc[3] = le32_to_cpu(key[3]);

	switch (key_len)
	{
	case AES_KEYSIZE_128:
	{
		uint32_t t{ctx.key_enc[3]};
		for (uint32_t i{0}; i < 10; ++i)
		{
			loop4(i);
		}
	}
	break;

	case AES_KEYSIZE_192:
	{
		ctx.key_enc[4] = le32_to_cpu(key[4]);
		uint32_t t{ctx.key_enc[5] = le32_to_cpu(key[5])};
		for (uint32_t i{0}; i < 8; ++i)
		{
			loop6(i);
		}
	}
	break;

	case AES_KEYSIZE_256:
	{
		ctx.key_enc[4] = le32_to_cpu(key[4]);
		ctx.key_enc[5] = le32_to_cpu(key[5]);
		ctx.key_enc[6] = le32_to_cpu(key[6]);
		uint32_t t{ctx.key_enc[7] = le32_to_cpu(key[7])};
		{
			uint32_t i{0};
			for (; i < 6; ++i)
			{
				loop8(i);
			}
			loop8tophalf(i);
		}
	}
	break;
	}

	ctx.key_dec[0] = ctx.key_enc[key_len + 24];
	ctx.key_dec[1] = ctx.key_enc[key_len + 25];
	ctx.key_dec[2] = ctx.key_enc[key_len + 26];
	ctx.key_dec[3] = ctx.key_enc[key_len + 27];

	for (uint32_t i{4}; i < key_len + 24; ++i)
	{
		[&ctx, &i](uint32_t j)
		{
			uint32_t u = star_x(ctx.key_enc[i]);
			uint32_t v = star_x(u);
			uint32_t w = star_x(v);
			uint32_t t = w ^ (ctx.key_enc[i]);
			ctx.key_dec[j] = u ^ v ^ w;
			ctx.key_dec[j] ^= aes_ror32(u ^ t, 8) ^ aes_ror32(v ^ t, 16) ^ aes_ror32(t, 24);
		}(key_len + 24 - (i & ~3) + (i & 3));
	}

	return 0;
}

/**
 * AES_SetKey - Set the AES key.
 * @ctx:		AES context struct.
 * @in_key:		The input key.
 * @key_len:	The size of the key.

	Returns 0 on success, on failure -1 is returned. The function uses AES_ExpandKey() to expand the key.
 */
static int AES_SetKey(aes_context_t& ctx, const uint8_t* in_key, const unsigned key_len)
{
	return AES_ExpandKey(ctx, in_key, key_len);
}

static void AES_Encrypt(aes_context_t& ctx, uint8_t* out, const uint8_t* in)
{
	const uint32_t* src{(const uint32_t*)in};
	uint32_t* dst{(uint32_t*)out};
	const uint32_t* kp{ctx.key_enc + 4};
	const int key_len{ctx.key_length};

	uint32_t b0[4];

	b0[0] = le32_to_cpu(src[0]) ^ ctx.key_enc[0];
	b0[1] = le32_to_cpu(src[1]) ^ ctx.key_enc[1];
	b0[2] = le32_to_cpu(src[2]) ^ ctx.key_enc[2];
	b0[3] = le32_to_cpu(src[3]) ^ ctx.key_enc[3];

	uint32_t b1[4];

	/* encrypt a block of text */
	auto f_rn = [](auto& bo, auto& bi, auto n, auto& k)
	{
		bo[n] = crypto_ft_tab[0][get_byte(bi[n], 0)]
			^ crypto_ft_tab[1][get_byte(bi[(n + 1) & 3], 1)]
			^ crypto_ft_tab[2][get_byte(bi[(n + 2) & 3], 2)]
			^ crypto_ft_tab[3][get_byte(bi[(n + 3) & 3], 3)]
			^ *(k + n);
	};

	auto f_nround = [&f_rn](auto& bo, auto& bi, auto& k)
	{
		f_rn(bo, bi, 0, k);
		f_rn(bo, bi, 1, k);
		f_rn(bo, bi, 2, k);
		f_rn(bo, bi, 3, k);
		k += 4;
	};

	if (key_len > 24)
	{
		f_nround(b1, b0, kp);
		f_nround(b0, b1, kp);
	}

	if (key_len > 16)
	{
		f_nround(b1, b0, kp);
		f_nround(b0, b1, kp);
	}

	f_nround(b1, b0, kp);
	f_nround(b0, b1, kp);
	f_nround(b1, b0, kp);
	f_nround(b0, b1, kp);
	f_nround(b1, b0, kp);
	f_nround(b0, b1, kp);
	f_nround(b1, b0, kp);
	f_nround(b0, b1, kp);
	f_nround(b1, b0, kp);

	/*
	#define f_rl(bo, bi, n, k)	do {	\
		bo[n] = crypto_fl_tab[0][get_byte(bi[n], 0)] ^	\
			crypto_fl_tab[1][get_byte(bi[(n + 1) & 3], 1)] ^	\
			crypto_fl_tab[2][get_byte(bi[(n + 2) & 3], 2)] ^	\
			crypto_fl_tab[3][get_byte(bi[(n + 3) & 3], 3)] ^ *(k + n);	\
	} while (0)

	#define f_lround(bo, bi, k)	do {	\
		f_rl(bo, bi, 0, k);	\
		f_rl(bo, bi, 1, k);	\
		f_rl(bo, bi, 2, k);	\
		f_rl(bo, bi, 3, k);	\
	} while (0)

	FIXME
	DOES k change? otherwise this simplifies to the following FOR loop
	*/
	for (uint32_t n; n < 4; ++n)
	{
		b0[n] = crypto_fl_tab[0][get_byte(b1[n], 0)]
			^ crypto_fl_tab[1][get_byte(b1[(n + 1) & 3], 1)]
			^ crypto_fl_tab[2][get_byte(b1[(n + 2) & 3], 2)]
			^ crypto_fl_tab[3][get_byte(b1[(n + 3) & 3], 3)]
			^ *(kp + n);
	}

	dst[0] = cpu_to_le32(b0[0]);
	dst[1] = cpu_to_le32(b0[1]);
	dst[2] = cpu_to_le32(b0[2]);
	dst[3] = cpu_to_le32(b0[3]);
}

static bool prng_enabled{false};
static aes_context_t prng_context;
static uint32_t prng_input_counter;
static uint32_t prng_values[4];
static unsigned prng_value_index{0};

// Initialize Pseudo-RNG using the specified 128-bit key.
void PRNG_Start(prng_seed_t key)
{
	AES_SetKey(prng_context, key, sizeof(prng_seed_t));
	prng_value_index = 4;
	prng_input_counter = 0;
	prng_enabled = true;
}

void PRNG_Stop()
{
	prng_enabled = false;
}

// Generate a set of new PRNG values by encrypting a new block.
static void PRNG_Generate()
{
	byte input[16];
	byte output[16];

	// Input for the cipher is a consecutively increasing 32-bit counter.
	for (size_t i{0}; i < 4; ++i)
	{
		input[4 * i] = prng_input_counter & 0xff;
		input[4 * i + 1] = (prng_input_counter >> 8) & 0xff;
		input[4 * i + 2] = (prng_input_counter >> 16) & 0xff;
		input[4 * i + 3] = (prng_input_counter >> 24) & 0xff;
		++prng_input_counter;
	}

	AES_Encrypt(prng_context, output, input);

	for (size_t i{0}; i < 4; ++i)
	{
		prng_values[i] = output[4 * i] | (output[4 * i + 1] << 8) | (output[4 * i + 2] << 16) | (output[4 * i + 3] << 24);
	}

	prng_value_index = 0;
}

// Read a random 32-bit integer from the PRNG.
unsigned PRNG_Random()
{
	if (!prng_enabled)
	{
		return 0;
	}

	if (prng_value_index >= 4)
	{
		PRNG_Generate();
	}

	unsigned result{prng_values[prng_value_index]};
	++prng_value_index;

	return result;
}
