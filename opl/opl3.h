/**********************************************************************************************************************************************\
	Copyright (C) 2013-2018 Alexey Khokholov (Nuke.YKT)

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	Nuked OPL3 emulator.
	Thanks:
		MAME Development Team(Jarek Burczynski, Tatsuyuki Satoh):	Feedback and Rhythm part calculation information.
		forums.submarine.org.uk(carbon14, opl3):					Tremolo and phase generator calculation information.
		OPLx decapsulated(Matthew Gambrell, Olli Niemitalo):		OPL2 ROMs.
		siliconpr0n.org(John McMaster, digshadow):					YMF262 and VRC VII decaps and die shots.

	version: 1.8
\**********************************************************************************************************************************************/
#pragma once

#include <array>
#include <vector>
#include <algorithm>
#include <ranges>

#include "../derma/common.h"
#include "../derma/enumbitset.h"
#include "../derma/bytetype.h"

namespace cudadoom::opl
{

constexpr size_t OPL_WRITEBUF_SIZE{1024};
constexpr size_t OPL_WRITEBUF_DELAY{2};

constexpr size_t RSM_FRAC{10};

//using Bitu = uintptr_t;
//using Bits = intptr_t;
//using Byte<uint64_t> = uint64_t;
//using Bit64s = int64_t;
//using Byte<uint32_t> = uint32_t;
//using Byte<int32_t> = int32_t;
//using Byte<uint16_t> = uint16_t;
//using Byte<int16_t> = int16_t;
//using Byte<uint8_t> = uint8_t;
//using Byte<int8_t> = int8_t;

struct opl3_channel;
struct opl3_chip;

// Channel types
enum
{
	ch_2op = 0,
	ch_4op = 1,
	ch_4op2 = 2,
	ch_drum = 3
};

// Envelope key types
enum
{
	egk_norm = 0x01,
	egk_drum = 0x02
};

enum class envelope_gen_num
{
	attack = 0,
	decay = 1,
	sustain = 2,
	release = 3
};

struct opl3_writebuf
{
	Byte<uint64_t> time;
	Byte<uint16_t> reg;
	Byte<uint8_t> data;
};

struct opl3_slot
{
	opl3_channel* channel;
	opl3_chip* chip;
	Byte<int16_t> out;
	Byte<int16_t> fbmod;
	Byte<int16_t>* mod;
	Byte<int16_t> prout;
	Byte<int16_t> eg_rout;
	Byte<int16_t> eg_out;
	Byte<uint8_t> eg_inc;
	envelope_gen_num eg_gen;
	Byte<uint8_t> eg_rate;
	Byte<uint8_t> eg_ksl;
	Byte<uint8_t>* trem;
	Byte<uint8_t> reg_vib;
	Byte<uint8_t> reg_type;
	Byte<uint8_t> reg_ksr;
	uint8_t reg_mult;
	uint8_t reg_ksl;
	Byte<uint8_t> reg_tl;
	Byte<uint8_t> reg_ar;
	Byte<uint8_t> reg_dr;
	Byte<uint8_t> reg_sl;
	Byte<uint8_t> reg_rr;
	Byte<uint8_t> reg_wf;
	Byte<uint8_t> key;
	Byte<uint32_t> pg_reset;
	Byte<uint32_t> pg_phase;
	Byte<uint16_t> pg_phase_out;
	Byte<uint8_t> slot_num;
};

struct opl3_channel
{
	std::array<opl3_slot*, 2> slots;
	opl3_channel* pair;
	opl3_chip* chip;
	std::array<Byte<int16_t>*, 4> out;
	Byte<uint8_t> chtype;
	Byte<uint16_t> f_num;
	Byte<uint8_t> block;
	Byte<uint8_t> fb;
	Byte<uint8_t> con;
	Byte<uint8_t> alg;
	Byte<uint8_t> ksv;
	Byte<uint16_t> cha;
	Byte<uint16_t> chb;
	Byte<uint8_t> ch_num;
};

struct opl3_chip
{
	std::array<opl3_channel, 18> channel;
	std::array<opl3_slot, 36> slot;
	Byte<uint16_t> timer;
	Byte<uint64_t> eg_timer;
	Byte<uint8_t> eg_timerrem;
	Byte<uint8_t> eg_state;
	Byte<uint8_t> eg_add;
	Byte<uint8_t> newm;
	Byte<uint8_t> nts;
	Byte<uint8_t> rhy;
	Byte<uint8_t> vibpos;
	Byte<uint8_t> vibshift;
	Byte<uint8_t> tremolo;
	Byte<uint8_t> tremolopos;
	Byte<uint8_t> tremoloshift;
	Byte<uint32_t> noise;
	Byte<int16_t> zeromod;
	std::array<Byte<int32_t>, 2> mixbuff;
	Byte<uint8_t> rm_hh_bit2;
	Byte<uint8_t> rm_hh_bit3;
	Byte<uint8_t> rm_hh_bit7;
	Byte<uint8_t> rm_hh_bit8;
	Byte<uint8_t> rm_tc_bit3;
	Byte<uint8_t> rm_tc_bit5;
	//OPL3L
	Byte<int32_t> rateratio;
	Byte<int32_t> samplecnt;
	std::array<Byte<int16_t>, 2> oldsamples;
	std::array<Byte<int16_t>, 2> samples;

	uint64_t writebuf_samplecnt;
	size_t writebuf_cur;
	size_t writebuf_last;
	TimeType writebuf_lasttime;
	std::array<opl3_writebuf, OPL_WRITEBUF_SIZE> writebuf;
};

// logsin table
static const std::array<Byte<uint16_t>, 256> logsinrom{
	Byte<uint16_t>(0x859), Byte<uint16_t>(0x6c3), Byte<uint16_t>(0x607), Byte<uint16_t>(0x58b),
	Byte<uint16_t>(0x52e), Byte<uint16_t>(0x4e4), Byte<uint16_t>(0x4a6), Byte<uint16_t>(0x471),
	Byte<uint16_t>(0x443), Byte<uint16_t>(0x41a), Byte<uint16_t>(0x3f5), Byte<uint16_t>(0x3d3),
	Byte<uint16_t>(0x3b5), Byte<uint16_t>(0x398), Byte<uint16_t>(0x37e), Byte<uint16_t>(0x365),
	Byte<uint16_t>(0x34e), Byte<uint16_t>(0x339), Byte<uint16_t>(0x324), Byte<uint16_t>(0x311),
	Byte<uint16_t>(0x2ff), Byte<uint16_t>(0x2ed), Byte<uint16_t>(0x2dc), Byte<uint16_t>(0x2cd),
	Byte<uint16_t>(0x2bd), Byte<uint16_t>(0x2af), Byte<uint16_t>(0x2a0), Byte<uint16_t>(0x293),
	Byte<uint16_t>(0x286), Byte<uint16_t>(0x279), Byte<uint16_t>(0x26d), Byte<uint16_t>(0x261),
	Byte<uint16_t>(0x256), Byte<uint16_t>(0x24b), Byte<uint16_t>(0x240), Byte<uint16_t>(0x236),
	Byte<uint16_t>(0x22c), Byte<uint16_t>(0x222), Byte<uint16_t>(0x218), Byte<uint16_t>(0x20f),
	Byte<uint16_t>(0x206), Byte<uint16_t>(0x1fd), Byte<uint16_t>(0x1f5), Byte<uint16_t>(0x1ec),
	Byte<uint16_t>(0x1e4), Byte<uint16_t>(0x1dc), Byte<uint16_t>(0x1d4), Byte<uint16_t>(0x1cd),
	Byte<uint16_t>(0x1c5), Byte<uint16_t>(0x1be), Byte<uint16_t>(0x1b7), Byte<uint16_t>(0x1b0),
	Byte<uint16_t>(0x1a9), Byte<uint16_t>(0x1a2), Byte<uint16_t>(0x19b), Byte<uint16_t>(0x195),
	Byte<uint16_t>(0x18f), Byte<uint16_t>(0x188), Byte<uint16_t>(0x182), Byte<uint16_t>(0x17c),
	Byte<uint16_t>(0x177), Byte<uint16_t>(0x171), Byte<uint16_t>(0x16b), Byte<uint16_t>(0x166),
	Byte<uint16_t>(0x160), Byte<uint16_t>(0x15b), Byte<uint16_t>(0x155), Byte<uint16_t>(0x150),
	Byte<uint16_t>(0x14b), Byte<uint16_t>(0x146), Byte<uint16_t>(0x141), Byte<uint16_t>(0x13c),
	Byte<uint16_t>(0x137), Byte<uint16_t>(0x133), Byte<uint16_t>(0x12e), Byte<uint16_t>(0x129),
	Byte<uint16_t>(0x125), Byte<uint16_t>(0x121), Byte<uint16_t>(0x11c), Byte<uint16_t>(0x118),
	Byte<uint16_t>(0x114), Byte<uint16_t>(0x10f), Byte<uint16_t>(0x10b), Byte<uint16_t>(0x107),
	Byte<uint16_t>(0x103), Byte<uint16_t>(0x0ff), Byte<uint16_t>(0x0fb), Byte<uint16_t>(0x0f8),
	Byte<uint16_t>(0x0f4), Byte<uint16_t>(0x0f0), Byte<uint16_t>(0x0ec), Byte<uint16_t>(0x0e9),
	Byte<uint16_t>(0x0e5), Byte<uint16_t>(0x0e2), Byte<uint16_t>(0x0de), Byte<uint16_t>(0x0db),
	Byte<uint16_t>(0x0d7), Byte<uint16_t>(0x0d4), Byte<uint16_t>(0x0d1), Byte<uint16_t>(0x0cd),
	Byte<uint16_t>(0x0ca), Byte<uint16_t>(0x0c7), Byte<uint16_t>(0x0c4), Byte<uint16_t>(0x0c1),
	Byte<uint16_t>(0x0be), Byte<uint16_t>(0x0bb), Byte<uint16_t>(0x0b8), Byte<uint16_t>(0x0b5),
	Byte<uint16_t>(0x0b2), Byte<uint16_t>(0x0af), Byte<uint16_t>(0x0ac), Byte<uint16_t>(0x0a9),
	Byte<uint16_t>(0x0a7), Byte<uint16_t>(0x0a4), Byte<uint16_t>(0x0a1), Byte<uint16_t>(0x09f),
	Byte<uint16_t>(0x09c), Byte<uint16_t>(0x099), Byte<uint16_t>(0x097), Byte<uint16_t>(0x094),
	Byte<uint16_t>(0x092), Byte<uint16_t>(0x08f), Byte<uint16_t>(0x08d), Byte<uint16_t>(0x08a),
	Byte<uint16_t>(0x088), Byte<uint16_t>(0x086), Byte<uint16_t>(0x083), Byte<uint16_t>(0x081),
	Byte<uint16_t>(0x07f), Byte<uint16_t>(0x07d), Byte<uint16_t>(0x07a), Byte<uint16_t>(0x078),
	Byte<uint16_t>(0x076), Byte<uint16_t>(0x074), Byte<uint16_t>(0x072), Byte<uint16_t>(0x070),
	Byte<uint16_t>(0x06e), Byte<uint16_t>(0x06c), Byte<uint16_t>(0x06a), Byte<uint16_t>(0x068),
	Byte<uint16_t>(0x066), Byte<uint16_t>(0x064), Byte<uint16_t>(0x062), Byte<uint16_t>(0x060),
	Byte<uint16_t>(0x05e), Byte<uint16_t>(0x05c), Byte<uint16_t>(0x05b), Byte<uint16_t>(0x059),
	Byte<uint16_t>(0x057), Byte<uint16_t>(0x055), Byte<uint16_t>(0x053), Byte<uint16_t>(0x052),
	Byte<uint16_t>(0x050), Byte<uint16_t>(0x04e), Byte<uint16_t>(0x04d), Byte<uint16_t>(0x04b),
	Byte<uint16_t>(0x04a), Byte<uint16_t>(0x048), Byte<uint16_t>(0x046), Byte<uint16_t>(0x045),
	Byte<uint16_t>(0x043), Byte<uint16_t>(0x042), Byte<uint16_t>(0x040), Byte<uint16_t>(0x03f),
	Byte<uint16_t>(0x03e), Byte<uint16_t>(0x03c), Byte<uint16_t>(0x03b), Byte<uint16_t>(0x039),
	Byte<uint16_t>(0x038), Byte<uint16_t>(0x037), Byte<uint16_t>(0x035), Byte<uint16_t>(0x034),
	Byte<uint16_t>(0x033), Byte<uint16_t>(0x031), Byte<uint16_t>(0x030), Byte<uint16_t>(0x02f),
	Byte<uint16_t>(0x02e), Byte<uint16_t>(0x02d), Byte<uint16_t>(0x02b), Byte<uint16_t>(0x02a),
	Byte<uint16_t>(0x029), Byte<uint16_t>(0x028), Byte<uint16_t>(0x027), Byte<uint16_t>(0x026),
	Byte<uint16_t>(0x025), Byte<uint16_t>(0x024), Byte<uint16_t>(0x023), Byte<uint16_t>(0x022),
	Byte<uint16_t>(0x021), Byte<uint16_t>(0x020), Byte<uint16_t>(0x01f), Byte<uint16_t>(0x01e),
	Byte<uint16_t>(0x01d), Byte<uint16_t>(0x01c), Byte<uint16_t>(0x01b), Byte<uint16_t>(0x01a),
	Byte<uint16_t>(0x019), Byte<uint16_t>(0x018), Byte<uint16_t>(0x017), Byte<uint16_t>(0x017),
	Byte<uint16_t>(0x016), Byte<uint16_t>(0x015), Byte<uint16_t>(0x014), Byte<uint16_t>(0x014),
	Byte<uint16_t>(0x013), Byte<uint16_t>(0x012), Byte<uint16_t>(0x011), Byte<uint16_t>(0x011),
	Byte<uint16_t>(0x010), Byte<uint16_t>(0x00f), Byte<uint16_t>(0x00f), Byte<uint16_t>(0x00e),
	Byte<uint16_t>(0x00d), Byte<uint16_t>(0x00d), Byte<uint16_t>(0x00c), Byte<uint16_t>(0x00c),
	Byte<uint16_t>(0x00b), Byte<uint16_t>(0x00a), Byte<uint16_t>(0x00a), Byte<uint16_t>(0x009),
	Byte<uint16_t>(0x009), Byte<uint16_t>(0x008), Byte<uint16_t>(0x008), Byte<uint16_t>(0x007),
	Byte<uint16_t>(0x007), Byte<uint16_t>(0x007), Byte<uint16_t>(0x006), Byte<uint16_t>(0x006),
	Byte<uint16_t>(0x005), Byte<uint16_t>(0x005), Byte<uint16_t>(0x005), Byte<uint16_t>(0x004),
	Byte<uint16_t>(0x004), Byte<uint16_t>(0x004), Byte<uint16_t>(0x003), Byte<uint16_t>(0x003),
	Byte<uint16_t>(0x003), Byte<uint16_t>(0x002), Byte<uint16_t>(0x002), Byte<uint16_t>(0x002),
	Byte<uint16_t>(0x002), Byte<uint16_t>(0x001), Byte<uint16_t>(0x001), Byte<uint16_t>(0x001),
	Byte<uint16_t>(0x001), Byte<uint16_t>(0x001), Byte<uint16_t>(0x001), Byte<uint16_t>(0x001),
	Byte<uint16_t>(0x000), Byte<uint16_t>(0x000), Byte<uint16_t>(0x000), Byte<uint16_t>(0x000),
	Byte<uint16_t>(0x000), Byte<uint16_t>(0x000), Byte<uint16_t>(0x000), Byte<uint16_t>(0x000)
};

// exp table
static const std::array<Byte<uint16_t>, 256> exprom{
	Byte<uint16_t>(0x7fa), Byte<uint16_t>(0x7f5), Byte<uint16_t>(0x7ef), Byte<uint16_t>(0x7ea),
	Byte<uint16_t>(0x7e4), Byte<uint16_t>(0x7df), Byte<uint16_t>(0x7da), Byte<uint16_t>(0x7d4),
	Byte<uint16_t>(0x7cf), Byte<uint16_t>(0x7c9), Byte<uint16_t>(0x7c4), Byte<uint16_t>(0x7bf),
	Byte<uint16_t>(0x7b9), Byte<uint16_t>(0x7b4), Byte<uint16_t>(0x7ae), Byte<uint16_t>(0x7a9),
	Byte<uint16_t>(0x7a4), Byte<uint16_t>(0x79f), Byte<uint16_t>(0x799), Byte<uint16_t>(0x794),
	Byte<uint16_t>(0x78f), Byte<uint16_t>(0x78a), Byte<uint16_t>(0x784), Byte<uint16_t>(0x77f),
	Byte<uint16_t>(0x77a), Byte<uint16_t>(0x775), Byte<uint16_t>(0x770), Byte<uint16_t>(0x76a),
	Byte<uint16_t>(0x765), Byte<uint16_t>(0x760), Byte<uint16_t>(0x75b), Byte<uint16_t>(0x756),
	Byte<uint16_t>(0x751), Byte<uint16_t>(0x74c), Byte<uint16_t>(0x747), Byte<uint16_t>(0x742),
	Byte<uint16_t>(0x73d), Byte<uint16_t>(0x738), Byte<uint16_t>(0x733), Byte<uint16_t>(0x72e),
	Byte<uint16_t>(0x729), Byte<uint16_t>(0x724), Byte<uint16_t>(0x71f), Byte<uint16_t>(0x71a),
	Byte<uint16_t>(0x715), Byte<uint16_t>(0x710), Byte<uint16_t>(0x70b), Byte<uint16_t>(0x706),
	Byte<uint16_t>(0x702), Byte<uint16_t>(0x6fd), Byte<uint16_t>(0x6f8), Byte<uint16_t>(0x6f3),
	Byte<uint16_t>(0x6ee), Byte<uint16_t>(0x6e9), Byte<uint16_t>(0x6e5), Byte<uint16_t>(0x6e0),
	Byte<uint16_t>(0x6db), Byte<uint16_t>(0x6d6), Byte<uint16_t>(0x6d2), Byte<uint16_t>(0x6cd),
	Byte<uint16_t>(0x6c8), Byte<uint16_t>(0x6c4), Byte<uint16_t>(0x6bf), Byte<uint16_t>(0x6ba),
	Byte<uint16_t>(0x6b5), Byte<uint16_t>(0x6b1), Byte<uint16_t>(0x6ac), Byte<uint16_t>(0x6a8),
	Byte<uint16_t>(0x6a3), Byte<uint16_t>(0x69e), Byte<uint16_t>(0x69a), Byte<uint16_t>(0x695),
	Byte<uint16_t>(0x691), Byte<uint16_t>(0x68c), Byte<uint16_t>(0x688), Byte<uint16_t>(0x683),
	Byte<uint16_t>(0x67f), Byte<uint16_t>(0x67a), Byte<uint16_t>(0x676), Byte<uint16_t>(0x671),
	Byte<uint16_t>(0x66d), Byte<uint16_t>(0x668), Byte<uint16_t>(0x664), Byte<uint16_t>(0x65f),
	Byte<uint16_t>(0x65b), Byte<uint16_t>(0x657), Byte<uint16_t>(0x652), Byte<uint16_t>(0x64e),
	Byte<uint16_t>(0x649), Byte<uint16_t>(0x645), Byte<uint16_t>(0x641), Byte<uint16_t>(0x63c),
	Byte<uint16_t>(0x638), Byte<uint16_t>(0x634), Byte<uint16_t>(0x630), Byte<uint16_t>(0x62b),
	Byte<uint16_t>(0x627), Byte<uint16_t>(0x623), Byte<uint16_t>(0x61e), Byte<uint16_t>(0x61a),
	Byte<uint16_t>(0x616), Byte<uint16_t>(0x612), Byte<uint16_t>(0x60e), Byte<uint16_t>(0x609),
	Byte<uint16_t>(0x605), Byte<uint16_t>(0x601), Byte<uint16_t>(0x5fd), Byte<uint16_t>(0x5f9),
	Byte<uint16_t>(0x5f5), Byte<uint16_t>(0x5f0), Byte<uint16_t>(0x5ec), Byte<uint16_t>(0x5e8),
	Byte<uint16_t>(0x5e4), Byte<uint16_t>(0x5e0), Byte<uint16_t>(0x5dc), Byte<uint16_t>(0x5d8),
	Byte<uint16_t>(0x5d4), Byte<uint16_t>(0x5d0), Byte<uint16_t>(0x5cc), Byte<uint16_t>(0x5c8),
	Byte<uint16_t>(0x5c4), Byte<uint16_t>(0x5c0), Byte<uint16_t>(0x5bc), Byte<uint16_t>(0x5b8),
	Byte<uint16_t>(0x5b4), Byte<uint16_t>(0x5b0), Byte<uint16_t>(0x5ac), Byte<uint16_t>(0x5a8),
	Byte<uint16_t>(0x5a4), Byte<uint16_t>(0x5a0), Byte<uint16_t>(0x59c), Byte<uint16_t>(0x599),
	Byte<uint16_t>(0x595), Byte<uint16_t>(0x591), Byte<uint16_t>(0x58d), Byte<uint16_t>(0x589),
	Byte<uint16_t>(0x585), Byte<uint16_t>(0x581), Byte<uint16_t>(0x57e), Byte<uint16_t>(0x57a),
	Byte<uint16_t>(0x576), Byte<uint16_t>(0x572), Byte<uint16_t>(0x56f), Byte<uint16_t>(0x56b),
	Byte<uint16_t>(0x567), Byte<uint16_t>(0x563), Byte<uint16_t>(0x560), Byte<uint16_t>(0x55c),
	Byte<uint16_t>(0x558), Byte<uint16_t>(0x554), Byte<uint16_t>(0x551), Byte<uint16_t>(0x54d),
	Byte<uint16_t>(0x549), Byte<uint16_t>(0x546), Byte<uint16_t>(0x542), Byte<uint16_t>(0x53e),
	Byte<uint16_t>(0x53b), Byte<uint16_t>(0x537), Byte<uint16_t>(0x534), Byte<uint16_t>(0x530),
	Byte<uint16_t>(0x52c), Byte<uint16_t>(0x529), Byte<uint16_t>(0x525), Byte<uint16_t>(0x522),
	Byte<uint16_t>(0x51e), Byte<uint16_t>(0x51b), Byte<uint16_t>(0x517), Byte<uint16_t>(0x514),
	Byte<uint16_t>(0x510), Byte<uint16_t>(0x50c), Byte<uint16_t>(0x509), Byte<uint16_t>(0x506),
	Byte<uint16_t>(0x502), Byte<uint16_t>(0x4ff), Byte<uint16_t>(0x4fb), Byte<uint16_t>(0x4f8),
	Byte<uint16_t>(0x4f4), Byte<uint16_t>(0x4f1), Byte<uint16_t>(0x4ed), Byte<uint16_t>(0x4ea),
	Byte<uint16_t>(0x4e7), Byte<uint16_t>(0x4e3), Byte<uint16_t>(0x4e0), Byte<uint16_t>(0x4dc),
	Byte<uint16_t>(0x4d9), Byte<uint16_t>(0x4d6), Byte<uint16_t>(0x4d2), Byte<uint16_t>(0x4cf),
	Byte<uint16_t>(0x4cc), Byte<uint16_t>(0x4c8), Byte<uint16_t>(0x4c5), Byte<uint16_t>(0x4c2),
	Byte<uint16_t>(0x4be), Byte<uint16_t>(0x4bb), Byte<uint16_t>(0x4b8), Byte<uint16_t>(0x4b5),
	Byte<uint16_t>(0x4b1), Byte<uint16_t>(0x4ae), Byte<uint16_t>(0x4ab), Byte<uint16_t>(0x4a8),
	Byte<uint16_t>(0x4a4), Byte<uint16_t>(0x4a1), Byte<uint16_t>(0x49e), Byte<uint16_t>(0x49b),
	Byte<uint16_t>(0x498), Byte<uint16_t>(0x494), Byte<uint16_t>(0x491), Byte<uint16_t>(0x48e),
	Byte<uint16_t>(0x48b), Byte<uint16_t>(0x488), Byte<uint16_t>(0x485), Byte<uint16_t>(0x482),
	Byte<uint16_t>(0x47e), Byte<uint16_t>(0x47b), Byte<uint16_t>(0x478), Byte<uint16_t>(0x475),
	Byte<uint16_t>(0x472), Byte<uint16_t>(0x46f), Byte<uint16_t>(0x46c), Byte<uint16_t>(0x469),
	Byte<uint16_t>(0x466), Byte<uint16_t>(0x463), Byte<uint16_t>(0x460), Byte<uint16_t>(0x45d),
	Byte<uint16_t>(0x45a), Byte<uint16_t>(0x457), Byte<uint16_t>(0x454), Byte<uint16_t>(0x451),
	Byte<uint16_t>(0x44e), Byte<uint16_t>(0x44b), Byte<uint16_t>(0x448), Byte<uint16_t>(0x445),
	Byte<uint16_t>(0x442), Byte<uint16_t>(0x43f), Byte<uint16_t>(0x43c), Byte<uint16_t>(0x439),
	Byte<uint16_t>(0x436), Byte<uint16_t>(0x433), Byte<uint16_t>(0x430), Byte<uint16_t>(0x42d),
	Byte<uint16_t>(0x42a), Byte<uint16_t>(0x428), Byte<uint16_t>(0x425), Byte<uint16_t>(0x422),
	Byte<uint16_t>(0x41f), Byte<uint16_t>(0x41c), Byte<uint16_t>(0x419), Byte<uint16_t>(0x416),
	Byte<uint16_t>(0x414), Byte<uint16_t>(0x411), Byte<uint16_t>(0x40e), Byte<uint16_t>(0x40b),
	Byte<uint16_t>(0x408), Byte<uint16_t>(0x406), Byte<uint16_t>(0x403), Byte<uint16_t>(0x400)
};
#include <cstddef>
// freq mult table multiplied by 2
// 1/2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 12, 12, 15, 15
static const std::array<Byte<uint8_t>, 16> mt{
			Byte<uint8_t>(1), Byte<uint8_t>(2), Byte<uint8_t>(4), Byte<uint8_t>(6),
			Byte<uint8_t>(8), Byte<uint8_t>(10), Byte<uint8_t>(12), Byte<uint8_t>(14),
			Byte<uint8_t>(16), Byte<uint8_t>(18), Byte<uint8_t>(20), Byte<uint8_t>(20),
			Byte<uint8_t>(24), Byte<uint8_t>(24), Byte<uint8_t>(30), Byte<uint8_t>(30) };

// ksl table
static const std::array<Byte<uint8_t>, 16> kslrom{
			Byte<uint8_t>(0), Byte<uint8_t>(32), Byte<uint8_t>(40), Byte<uint8_t>(45),
			Byte<uint8_t>(48), Byte<uint8_t>(51), Byte<uint8_t>(53), Byte<uint8_t>(55),
			Byte<uint8_t>(56), Byte<uint8_t>(58), Byte<uint8_t>(59), Byte<uint8_t>(60),
			Byte<uint8_t>(61), Byte<uint8_t>(62), Byte<uint8_t>(63), Byte<uint8_t>(64) };

static const std::array<Byte<uint8_t>, 4> kslshift{ Byte<uint8_t>(8), Byte<uint8_t>(1), Byte<uint8_t>(2), Byte<uint8_t>(0) };

// envelope generator constants
static const std::array<std::array<Byte<uint8_t>, 4>, 4> eg_incstep{
	{
			{ Byte<uint8_t>(0), Byte<uint8_t>(0), Byte<uint8_t>(0), Byte<uint8_t>(0) },
			{ Byte<uint8_t>(1), Byte<uint8_t>(0), Byte<uint8_t>(0), Byte<uint8_t>(0) },
			{ Byte<uint8_t>(1), Byte<uint8_t>(0), Byte<uint8_t>(1), Byte<uint8_t>(0) },
			{ Byte<uint8_t>(1), Byte<uint8_t>(1), Byte<uint8_t>(1), Byte<uint8_t>(0) }
	}
};

// address decoding
static const std::array<Byte<int8_t>, 32> ad_slot{ 0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, 9, 10, 11, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const std::array<Byte<uint8_t>, 18> ch_slot{
			Byte<uint8_t>(0), Byte<uint8_t>(1), Byte<uint8_t>(2), Byte<uint8_t>(6), Byte<uint8_t>(7), Byte<uint8_t>(8),
			Byte<uint8_t>(12), Byte<uint8_t>(13), Byte<uint8_t>(14), Byte<uint8_t>(18), Byte<uint8_t>(19), Byte<uint8_t>(20),
			Byte<uint8_t>(24), Byte<uint8_t>(25), Byte<uint8_t>(26), Byte<uint8_t>(30), Byte<uint8_t>(31), Byte<uint8_t>(32) };

// Envelope generator
typedef Byte<int16_t>(*envelope_sinfunc)(Byte<uint16_t> phase, Byte<uint16_t> envelope);
typedef void(*envelope_genfunc)(opl3_slot* slott);

//void OPL3_Generate(opl3_chip* chip, Byte<int16_t>* buf);
//void OPL3_GenerateResampled(opl3_chip* chip, Byte<int16_t>* buf);
//void OPL3_Reset(opl3_chip* chip, Byte<uint32_t> samplerate);
//void OPL3_WriteReg(opl3_chip* chip, Byte<uint16_t> reg, Byte<uint8_t> v);
//void OPL3_WriteRegBuffered(opl3_chip* chip, Byte<uint16_t> reg, Byte<uint8_t> v);
//void OPL3_GenerateStream(opl3_chip* chip, Byte<int16_t>* sndptr, Byte<uint32_t> numsamples);

static Byte<int16_t> OPL3_EnvelopeCalcExp(Byte<uint32_t> level)
{
	if (level > 0x1fff)
	{
		level = 0x1fff;
	}

	return (exprom[level & 0xff] << 1) >> (level >> 8);
}

static Byte<int16_t> OPL3_EnvelopeCalcSin0(Byte<uint16_t> phase, Byte<uint16_t> envelope)
{
	Byte<uint16_t> out{0};
	Byte<uint16_t> neg{0};
	phase &= 0x3ff;
	if (phase & 0x200)
	{
		neg = 0xffff;
	}

	if (phase & 0x100)
	{
		out = logsinrom[(phase & 0xff) ^ 0xff];
	}
	else
	{
		out = logsinrom[phase & 0xff];
	}

	return OPL3_EnvelopeCalcExp(out + (envelope << 3)) ^ neg;
}

static Byte<int16_t> OPL3_EnvelopeCalcSin1(Byte<uint16_t> phase, Byte<uint16_t> envelope)
{
	Byte<uint16_t> out{0};
	phase &= 0x3ff;
	if (phase & 0x200)
	{
		out = 0x1000;
	}
	else if (phase & 0x100)
	{
		out = logsinrom[(phase & 0xff) ^ 0xff];
	}
	else
	{
		out = logsinrom[phase & 0xff];
	}

	return OPL3_EnvelopeCalcExp(out + (envelope << 3));
}

static Byte<int16_t> OPL3_EnvelopeCalcSin2(Byte<uint16_t> phase, Byte<uint16_t> envelope)
{
	Byte<uint16_t> out{0};
	phase &= 0x3ff;
	if (phase & 0x100)
	{
		out = logsinrom[(phase & 0xff) ^ 0xff];
	}
	else
	{
		out = logsinrom[phase & 0xff];
	}

	return OPL3_EnvelopeCalcExp(out + (envelope << 3));
}

static Byte<int16_t> OPL3_EnvelopeCalcSin3(Byte<uint16_t> phase, Byte<uint16_t> envelope)
{
	Byte<uint16_t> out{0};
	phase &= 0x3ff;
	if (phase & 0x100)
	{
		out = 0x1000;
	}
	else
	{
		out = logsinrom[phase & 0xff];
	}

	return OPL3_EnvelopeCalcExp(out + (envelope << 3));
}

static Byte<int16_t> OPL3_EnvelopeCalcSin4(Byte<uint16_t> phase, Byte<uint16_t> envelope)
{
	Byte<uint16_t> out{0};
	Byte<uint16_t> neg{0};
	phase &= 0x3ff;
	if ((phase & 0x300) == 0x100)
	{
		neg = 0xffff;
	}

	if (phase & 0x200)
	{
		out = 0x1000;
	}
	else if (phase & 0x80)
	{
		out = logsinrom[((phase ^ 0xff) << 1) & 0xff];
	}
	else
	{
		out = logsinrom[(phase << 1) & 0xff];
	}

	return OPL3_EnvelopeCalcExp(out + (envelope << 3)) ^ neg;
}

static Byte<int16_t> OPL3_EnvelopeCalcSin5(Byte<uint16_t> phase, Byte<uint16_t> envelope)
{
	Byte<uint16_t> out{0};
	phase &= 0x3ff;
	if (phase & 0x200)
	{
		out = 0x1000;
	}
	else if (phase & 0x80)
	{
		out = logsinrom[((phase ^ 0xff) << 1) & 0xff];
	}
	else
	{
		out = logsinrom[(phase << 1) & 0xff];
	}

	return OPL3_EnvelopeCalcExp(out + (envelope << 3));
}

static Byte<int16_t> OPL3_EnvelopeCalcSin6(Byte<uint16_t> phase, Byte<uint16_t> envelope)
{
	Byte<uint16_t> neg{0};
	phase &= 0x3ff;
	if (phase & 0x200)
	{
		neg = 0xffff;
	}

	return OPL3_EnvelopeCalcExp(envelope << 3) ^ neg;
}

static Byte<int16_t> OPL3_EnvelopeCalcSin7(Byte<uint16_t> phase, Byte<uint16_t> envelope)
{
	Byte<uint16_t> out{0};
	Byte<uint16_t> neg{0};
	phase &= 0x3ff;
	if (phase & 0x200)
	{
		neg = 0xffff;
		phase = (phase & 0x1ff) ^ 0x1ff;
	}

	out = phase << 3;

	return OPL3_EnvelopeCalcExp(out + (envelope << 3)) ^ neg;
}

static void OPL3_EnvelopeUpdateKSL(opl3_slot* slot)
{
	Byte<int16_t> ksl = Byte<uint8_t,1>((kslrom[slot->channel->f_num >> 6] << 2)) - ((0x08 - slot->channel->block) << 5);
	if (ksl < 0)
	{
		ksl = 0;
	}
	slot->eg_ksl = static_cast<uint8_t>(ksl.bits);
}

static const std::array<envelope_sinfunc, 8> envelope_sin{
	OPL3_EnvelopeCalcSin0,
	OPL3_EnvelopeCalcSin1,
	OPL3_EnvelopeCalcSin2,
	OPL3_EnvelopeCalcSin3,
	OPL3_EnvelopeCalcSin4,
	OPL3_EnvelopeCalcSin5,
	OPL3_EnvelopeCalcSin6,
	OPL3_EnvelopeCalcSin7
};

static void OPL3_EnvelopeCalc(opl3_slot* slot)
{
	Byte<uint8_t> nonzero;
	Byte<uint8_t> rate;
	Byte<uint8_t> rate_hi;
	Byte<uint8_t> rate_lo;
	Byte<uint8_t> reg_rate{0};
	Byte<uint8_t> ks;
	uint8_t eg_shift;
	Byte<uint8_t> shift;
	Byte<uint16_t> eg_rout;
	Byte<int16_t> eg_inc;
	Byte<uint8_t> eg_off;
	Byte<uint8_t> reset{0};
	slot->eg_out = slot->eg_rout + (slot->reg_tl << 2) + (slot->eg_ksl >> kslshift[slot->reg_ksl]) + *slot->trem;
	if (slot->key && slot->eg_gen == envelope_gen_num::release)
	{
		reset = 1;
		reg_rate = slot->reg_ar;
	}
	else
	{
		switch (slot->eg_gen)
		{
		case envelope_gen_num::attack:
			reg_rate = slot->reg_ar;
			break;
		case envelope_gen_num::decay:
			reg_rate = slot->reg_dr;
			break;
		case envelope_gen_num::sustain:
			if (!slot->reg_type)
			{
				reg_rate = slot->reg_rr;
			}
			break;
		case envelope_gen_num::release:
			reg_rate = slot->reg_rr;
			break;
		}
	}
	slot->pg_reset = reset;
	ks = slot->channel->ksv >> ((slot->reg_ksr ^ 1) << 1);
	nonzero = (reg_rate != 0);
	rate = ks + (reg_rate << 2);
	rate_hi = rate >> 2;
	rate_lo = rate & 0x03;
	if (rate_hi & 0x10)
	{
		rate_hi = 0x0f;
	}
	eg_shift = rate_hi + slot->chip->eg_add;
	shift = 0;
	if (nonzero)
	{
		if (rate_hi < 12)
		{
			if (slot->chip->eg_state)
			{
				switch (eg_shift)
				{
				case 12:
					shift = 1;
					break;
				case 13:
					shift = (rate_lo >> 1) & 0x01;
					break;
				case 14:
					shift = rate_lo & 0x01;
					break;
				default:
					break;
				}
			}
		}
		else
		{
			shift = (rate_hi & 0x03) + eg_incstep[rate_lo][slot->chip->timer & 0x03];
			if (shift & 0x04)
			{
				shift = 0x03;
			}
			if (!shift)
			{
				shift = slot->chip->eg_state;
			}
		}
	}
	eg_rout = slot->eg_rout;
	eg_inc = 0;
	eg_off = 0;
	// Instant attack
	if (reset && rate_hi == Byte<uint8_t>(0x0f))
	{
		eg_rout = 0x00;
	}
	// Envelope off
	if ((slot->eg_rout & 0x1f8) == 0x1f8)
	{
		eg_off = 1;
	}
	if (slot->eg_gen != envelope_gen_num::attack && !reset && eg_off)
	{
		eg_rout = 0x1ff;
	}
	switch (slot->eg_gen)
	{
	case envelope_gen_num::attack:
		if (!slot->eg_rout)
		{
			slot->eg_gen = envelope_gen_num::decay;
		}
		else if (slot->key && shift > 0 && rate_hi != Byte<uint8_t>(0x0f))
		{
			eg_inc = ((~slot->eg_rout) << shift) >> 4;
		}
		break;
	case envelope_gen_num::decay:
		if ((slot->eg_rout >> 4) == slot->reg_sl)
		{
			slot->eg_gen = envelope_gen_num::sustain;
		}
		else if (!eg_off && !reset && shift > 0)
		{
			eg_inc = 1 << (shift - 1);
		}
		break;
	case envelope_gen_num::sustain:
	case envelope_gen_num::release:
		if (!eg_off && !reset && shift > 0)
		{
			eg_inc = 1 << (shift - 1);
		}
		break;
	}
	slot->eg_rout = (eg_rout + eg_inc) & 0x1ff;
	// Key off
	if (reset)
	{
		slot->eg_gen = envelope_gen_num::attack;
	}
	if (!slot->key)
	{
		slot->eg_gen = envelope_gen_num::release;
	}
}

static void OPL3_EnvelopeKeyOn(opl3_slot* slot, Byte<uint8_t> type)
{
	slot->key |= type;
}

static void OPL3_EnvelopeKeyOff(opl3_slot* slot, Byte<uint8_t> type)
{
	slot->key &= ~type;
}

// Phase Generator
static void OPL3_PhaseGenerate(opl3_slot* slot)
{
	opl3_chip* chip;
	Byte<uint16_t> f_num;
	Byte<uint32_t> basefreq;
	Byte<uint8_t> rm_xor;
	Byte<uint8_t> n_bit;
	Byte<uint32_t> noise;
	Byte<uint16_t> phase;

	chip = slot->chip;
	f_num = slot->channel->f_num;
	if (slot->reg_vib)
	{
		Byte<int8_t> range;
		Byte<uint8_t> vibpos;

		range = (f_num >> 7) & 7;
		vibpos = slot->chip->vibpos;

		if (!(vibpos & 3))
		{
			range = 0;
		}
		else if (vibpos & 1)
		{
			range >>= 1;
		}
		range >>= slot->chip->vibshift;

		if (vibpos & 4)
		{
			range = -range;
		}
		f_num += range;
	}
	basefreq = (f_num << slot->channel->block) >> 1;
	phase = (Byte<uint16_t>)(slot->pg_phase >> 9);
	if (slot->pg_reset)
	{
		slot->pg_phase = 0;
	}
	slot->pg_phase += (basefreq * mt[slot->reg_mult]) >> 1;
	// Rhythm mode
	noise = chip->noise;
	slot->pg_phase_out = phase;
	if (slot->slot_num == 13) // hh
	{
		chip->rm_hh_bit2 = (phase >> 2) & 1;
		chip->rm_hh_bit3 = (phase >> 3) & 1;
		chip->rm_hh_bit7 = (phase >> 7) & 1;
		chip->rm_hh_bit8 = (phase >> 8) & 1;
	}
	if (slot->slot_num == 17 && (chip->rhy & 0x20)) // tc
	{
		chip->rm_tc_bit3 = (phase >> 3) & 1;
		chip->rm_tc_bit5 = (phase >> 5) & 1;
	}
	if (chip->rhy & 0x20)
	{
		rm_xor = (chip->rm_hh_bit2 ^ chip->rm_hh_bit7)
				| (chip->rm_hh_bit3 ^ chip->rm_tc_bit5)
				| (chip->rm_tc_bit3 ^ chip->rm_tc_bit5);
		switch (slot->slot_num)
		{
		case 13: // hh
			slot->pg_phase_out = rm_xor << 9;
			if (rm_xor ^ (noise & 1))
			{
				slot->pg_phase_out |= 0xd0;
			}
			else
			{
				slot->pg_phase_out |= 0x34;
			}
			break;
		case 16: // sd
			slot->pg_phase_out = (chip->rm_hh_bit8 << 9) | ((chip->rm_hh_bit8 ^ (noise & 1)) << 8);
			break;
		case 17: // tc
			slot->pg_phase_out = (rm_xor << 9) | 0x80;
			break;
		default:
			break;
		}
	}
	n_bit = ((noise >> 14) ^ noise) & 0x01;
	chip->noise = (noise >> 1) | (n_bit << 22);
}

// Slot
static void OPL3_SlotWrite20(opl3_slot* slot, Byte<uint8_t> data)
{
	if ((data >> 7) & 0x01)
	{
		slot->trem = &slot->chip->tremolo;
	}
	else
	{
		slot->trem = (std::byte*)&slot->chip->zeromod;
	}
	slot->reg_vib = (data >> 6) & 0x01;
	slot->reg_type = (data >> 5) & 0x01;
	slot->reg_ksr = (data >> 4) & 0x01;
	slot->reg_mult = data & 0x0f;
}

static void OPL3_SlotWrite40(opl3_slot* slot, Byte<uint8_t> data)
{
	slot->reg_ksl = (data >> 6) & 0x03;
	slot->reg_tl = data & 0x3f;
	OPL3_EnvelopeUpdateKSL(slot);
}

static void OPL3_SlotWrite60(opl3_slot* slot, Byte<uint8_t> data)
{
	slot->reg_ar = (data >> 4) & 0x0f;
	slot->reg_dr = data & 0x0f;
}

static void OPL3_SlotWrite80(opl3_slot* slot, Byte<uint8_t> data)
{
	slot->reg_sl = (data >> 4) & 0x0f;
	if (slot->reg_sl == Byte<uint8_t>(0x0f))
	{
		slot->reg_sl = 0x1f;
	}
	slot->reg_rr = data & 0x0f;
}

static void OPL3_SlotWriteE0(opl3_slot* slot, Byte<uint8_t> data)
{
	slot->reg_wf = data & 0x07;
	if (slot->chip->newm == Byte<uint8_t>(0x00))
	{
		slot->reg_wf &= 0x03;
	}
}

static void OPL3_SlotGenerate(opl3_slot* slot)
{
	slot->out = envelope_sin[slot->reg_wf](slot->pg_phase_out + *slot->mod, slot->eg_out);
}

static void OPL3_SlotCalcFB(opl3_slot* slot)
{
	if (slot->channel->fb != Byte<uint8_t>(0x00))
	{
		slot->fbmod = (slot->prout + slot->out) >> (Byte<uint8_t>(0x09) - slot->channel->fb);
	}
	else
	{
		slot->fbmod = 0;
	}
	slot->prout = slot->out;
}

// Channel
static void OPL3_ChannelSetupAlg(opl3_channel* channel);

static void OPL3_ChannelUpdateRhythm(opl3_chip* chip, Byte<uint8_t> data)
{
	chip->rhy = data & 0x3f;
	if (chip->rhy & 0x20)
	{
		opl3_channel* channel6 = &chip->channel[6];
		opl3_channel* channel7 = &chip->channel[7];
		opl3_channel* channel8 = &chip->channel[8];
		channel6->out[0] = &channel6->slots[1]->out;
		channel6->out[1] = &channel6->slots[1]->out;
		channel6->out[2] = &chip->zeromod;
		channel6->out[3] = &chip->zeromod;
		channel7->out[0] = &channel7->slots[0]->out;
		channel7->out[1] = &channel7->slots[0]->out;
		channel7->out[2] = &channel7->slots[1]->out;
		channel7->out[3] = &channel7->slots[1]->out;
		channel8->out[0] = &channel8->slots[0]->out;
		channel8->out[1] = &channel8->slots[0]->out;
		channel8->out[2] = &channel8->slots[1]->out;
		channel8->out[3] = &channel8->slots[1]->out;
		for (size_t chnum{6}; chnum < 9; ++chnum)
		{
			chip->channel[chnum].chtype = ch_drum;
		}
		OPL3_ChannelSetupAlg(channel6);
		OPL3_ChannelSetupAlg(channel7);
		OPL3_ChannelSetupAlg(channel8);
		//hh
		if (chip->rhy & 0x01)
		{
			OPL3_EnvelopeKeyOn(channel7->slots[0], egk_drum);
		}
		else
		{
			OPL3_EnvelopeKeyOff(channel7->slots[0], egk_drum);
		}
		//tc
		if (chip->rhy & 0x02)
		{
			OPL3_EnvelopeKeyOn(channel8->slots[1], egk_drum);
		}
		else
		{
			OPL3_EnvelopeKeyOff(channel8->slots[1], egk_drum);
		}
		//tom
		if (chip->rhy & 0x04)
		{
			OPL3_EnvelopeKeyOn(channel8->slots[0], egk_drum);
		}
		else
		{
			OPL3_EnvelopeKeyOff(channel8->slots[0], egk_drum);
		}
		//sd
		if (chip->rhy & 0x08)
		{
			OPL3_EnvelopeKeyOn(channel7->slots[1], egk_drum);
		}
		else
		{
			OPL3_EnvelopeKeyOff(channel7->slots[1], egk_drum);
		}
		//bd
		if (chip->rhy & 0x10)
		{
			OPL3_EnvelopeKeyOn(channel6->slots[0], egk_drum);
			OPL3_EnvelopeKeyOn(channel6->slots[1], egk_drum);
		}
		else
		{
			OPL3_EnvelopeKeyOff(channel6->slots[0], egk_drum);
			OPL3_EnvelopeKeyOff(channel6->slots[1], egk_drum);
		}
	}
	else
	{
		for (size_t chnum{6}; chnum < 9; ++chnum)
		{
			chip->channel[chnum].chtype = ch_2op;
			OPL3_ChannelSetupAlg(&chip->channel[chnum]);
			OPL3_EnvelopeKeyOff(chip->channel[chnum].slots[0], egk_drum);
			OPL3_EnvelopeKeyOff(chip->channel[chnum].slots[1], egk_drum);
		}
	}
}

static void OPL3_ChannelWriteA0(opl3_channel* channel, Byte<uint8_t> data)
{
	if (channel->chip->newm && channel->chtype == ch_4op2)
	{
		return;
	}
	channel->f_num = (channel->f_num & 0x300) | data;
	channel->ksv = (channel->block << 1) | ((channel->f_num >> (Byte<uint8_t>(0x09) - channel->chip->nts)) & 0x01);
	OPL3_EnvelopeUpdateKSL(channel->slots[0]);
	OPL3_EnvelopeUpdateKSL(channel->slots[1]);
	if (channel->chip->newm && channel->chtype == ch_4op)
	{
		channel->pair->f_num = channel->f_num;
		channel->pair->ksv = channel->ksv;
		OPL3_EnvelopeUpdateKSL(channel->pair->slots[0]);
		OPL3_EnvelopeUpdateKSL(channel->pair->slots[1]);
	}
}

static void OPL3_ChannelWriteB0(opl3_channel* channel, Byte<uint8_t> data)
{
	if (channel->chip->newm && channel->chtype == ch_4op2)
	{
		return;
	}
	channel->f_num = (channel->f_num & 0xffu) | ((data & 0x03) << 8);
	channel->block = (data >> 2) & 0x07;
	channel->ksv = (channel->block << 1) | ((channel->f_num >> (Byte<uint8_t>(0x09) - channel->chip->nts)) & 0x01);
	OPL3_EnvelopeUpdateKSL(channel->slots[0]);
	OPL3_EnvelopeUpdateKSL(channel->slots[1]);
	if (channel->chip->newm && channel->chtype == ch_4op)
	{
		channel->pair->f_num = channel->f_num;
		channel->pair->block = channel->block;
		channel->pair->ksv = channel->ksv;
		OPL3_EnvelopeUpdateKSL(channel->pair->slots[0]);
		OPL3_EnvelopeUpdateKSL(channel->pair->slots[1]);
	}
}

static void OPL3_ChannelSetupAlg(opl3_channel* channel)
{
	if (channel->chtype == ch_drum)
	{
		if (channel->ch_num == 7 || channel->ch_num == 8)
		{
			channel->slots[0]->mod = &channel->chip->zeromod;
			channel->slots[1]->mod = &channel->chip->zeromod;
			return;
		}
		switch (channel->alg & 0x01)
		{
		case 0x00:
			channel->slots[0]->mod = &channel->slots[0]->fbmod;
			channel->slots[1]->mod = &channel->slots[0]->out;
			break;
		case 0x01:
			channel->slots[0]->mod = &channel->slots[0]->fbmod;
			channel->slots[1]->mod = &channel->chip->zeromod;
			break;
		}
		return;
	}
	if (channel->alg & 0x08)
	{
		return;
	}
	if (channel->alg & 0x04)
	{
		channel->pair->out[0] = &channel->chip->zeromod;
		channel->pair->out[1] = &channel->chip->zeromod;
		channel->pair->out[2] = &channel->chip->zeromod;
		channel->pair->out[3] = &channel->chip->zeromod;
		switch (channel->alg & 0x03)
		{
		case 0x00:
			channel->pair->slots[0]->mod = &channel->pair->slots[0]->fbmod;
			channel->pair->slots[1]->mod = &channel->pair->slots[0]->out;
			channel->slots[0]->mod = &channel->pair->slots[1]->out;
			channel->slots[1]->mod = &channel->slots[0]->out;
			channel->out[0] = &channel->slots[1]->out;
			channel->out[1] = &channel->chip->zeromod;
			channel->out[2] = &channel->chip->zeromod;
			channel->out[3] = &channel->chip->zeromod;
			break;
		case 0x01:
			channel->pair->slots[0]->mod = &channel->pair->slots[0]->fbmod;
			channel->pair->slots[1]->mod = &channel->pair->slots[0]->out;
			channel->slots[0]->mod = &channel->chip->zeromod;
			channel->slots[1]->mod = &channel->slots[0]->out;
			channel->out[0] = &channel->pair->slots[1]->out;
			channel->out[1] = &channel->slots[1]->out;
			channel->out[2] = &channel->chip->zeromod;
			channel->out[3] = &channel->chip->zeromod;
			break;
		case 0x02:
			channel->pair->slots[0]->mod = &channel->pair->slots[0]->fbmod;
			channel->pair->slots[1]->mod = &channel->chip->zeromod;
			channel->slots[0]->mod = &channel->pair->slots[1]->out;
			channel->slots[1]->mod = &channel->slots[0]->out;
			channel->out[0] = &channel->pair->slots[0]->out;
			channel->out[1] = &channel->slots[1]->out;
			channel->out[2] = &channel->chip->zeromod;
			channel->out[3] = &channel->chip->zeromod;
			break;
		case 0x03:
			channel->pair->slots[0]->mod = &channel->pair->slots[0]->fbmod;
			channel->pair->slots[1]->mod = &channel->chip->zeromod;
			channel->slots[0]->mod = &channel->pair->slots[1]->out;
			channel->slots[1]->mod = &channel->chip->zeromod;
			channel->out[0] = &channel->pair->slots[0]->out;
			channel->out[1] = &channel->slots[0]->out;
			channel->out[2] = &channel->slots[1]->out;
			channel->out[3] = &channel->chip->zeromod;
			break;
		}
	}
	else
	{
		switch (channel->alg & 0x01)
		{
		case 0x00:
			channel->slots[0]->mod = &channel->slots[0]->fbmod;
			channel->slots[1]->mod = &channel->slots[0]->out;
			channel->out[0] = &channel->slots[1]->out;
			channel->out[1] = &channel->chip->zeromod;
			channel->out[2] = &channel->chip->zeromod;
			channel->out[3] = &channel->chip->zeromod;
			break;
		case 0x01:
			channel->slots[0]->mod = &channel->slots[0]->fbmod;
			channel->slots[1]->mod = &channel->chip->zeromod;
			channel->out[0] = &channel->slots[0]->out;
			channel->out[1] = &channel->slots[1]->out;
			channel->out[2] = &channel->chip->zeromod;
			channel->out[3] = &channel->chip->zeromod;
			break;
		}
	}
}

static void OPL3_ChannelWriteC0(opl3_channel* channel, Byte<uint8_t> data)
{
	channel->fb = (data & 0x0e) >> 1;
	channel->con = data & 0x01;
	channel->alg = channel->con;
	if (channel->chip->newm)
	{
		if (channel->chtype == ch_4op)
		{
			channel->pair->alg = 0x04 | (channel->con << 1) | (channel->pair->con);
			channel->alg = 0x08;
			OPL3_ChannelSetupAlg(channel->pair);
		}
		else if (channel->chtype == ch_4op2)
		{
			channel->alg = 0x04 | (channel->pair->con << 1) | (channel->con);
			channel->pair->alg = 0x08;
			OPL3_ChannelSetupAlg(channel);
		}
		else
		{
			OPL3_ChannelSetupAlg(channel);
		}
	}
	else
	{
		OPL3_ChannelSetupAlg(channel);
	}
	if (channel->chip->newm)
	{
		channel->cha = ((data >> 4) & 0x01) ? ~0 : 0;
		channel->chb = ((data >> 5) & 0x01) ? ~0 : 0;
	}
	else
	{
		channel->cha = channel->chb = static_cast<Byte<uint16_t>>(~0);
	}
}

static void OPL3_ChannelKeyOn(opl3_channel* channel)
{
	if (channel->chip->newm)
	{
		if (channel->chtype == ch_4op)
		{
			OPL3_EnvelopeKeyOn(channel->slots[0], egk_norm);
			OPL3_EnvelopeKeyOn(channel->slots[1], egk_norm);
			OPL3_EnvelopeKeyOn(channel->pair->slots[0], egk_norm);
			OPL3_EnvelopeKeyOn(channel->pair->slots[1], egk_norm);
		}
		else if (channel->chtype == ch_2op || channel->chtype == ch_drum)
		{
			OPL3_EnvelopeKeyOn(channel->slots[0], egk_norm);
			OPL3_EnvelopeKeyOn(channel->slots[1], egk_norm);
		}
	}
	else
	{
		OPL3_EnvelopeKeyOn(channel->slots[0], egk_norm);
		OPL3_EnvelopeKeyOn(channel->slots[1], egk_norm);
	}
}

static void OPL3_ChannelKeyOff(opl3_channel* channel)
{
	if (channel->chip->newm)
	{
		if (channel->chtype == ch_4op)
		{
			OPL3_EnvelopeKeyOff(channel->slots[0], egk_norm);
			OPL3_EnvelopeKeyOff(channel->slots[1], egk_norm);
			OPL3_EnvelopeKeyOff(channel->pair->slots[0], egk_norm);
			OPL3_EnvelopeKeyOff(channel->pair->slots[1], egk_norm);
		}
		else if (channel->chtype == ch_2op || channel->chtype == ch_drum)
		{
			OPL3_EnvelopeKeyOff(channel->slots[0], egk_norm);
			OPL3_EnvelopeKeyOff(channel->slots[1], egk_norm);
		}
	}
	else
	{
		OPL3_EnvelopeKeyOff(channel->slots[0], egk_norm);
		OPL3_EnvelopeKeyOff(channel->slots[1], egk_norm);
	}
}

static void OPL3_ChannelSet4Op(opl3_chip* chip, Byte<uint8_t> data)
{
	for (size_t bit{0}; bit < 6; ++bit)
	{
		size_t chnum{bit};
		if (bit >= 3)
		{
			chnum += 9 - 3;
		}
		if ((data >> bit) & 0x01)
		{
			chip->channel[chnum].chtype = ch_4op;
			chip->channel[chnum + 3].chtype = ch_4op2;
		}
		else
		{
			chip->channel[chnum].chtype = ch_2op;
			chip->channel[chnum + 3].chtype = ch_2op;
		}
	}
}

static Byte<int16_t> OPL3_ClipSample(Byte<int32_t> sample)
{
	if (sample > 32767)
	{
		sample = 32767;
	}
	else if (sample < -32768)
	{
		sample = -32768;
	}

	// SAFE NARROWING
	return static_cast<Byte<int16_t>>(sample);
}

void OPL3_WriteReg(opl3_chip* chip, Byte<uint16_t> reg, Byte<uint8_t> v)
{
	Byte<uint8_t> high{(reg >> 8) & 0x01};
	Byte<uint8_t> regm{reg & 0xff};
	switch (regm & 0xf0)
	{
	case 0x00:
		if (high)
		{
			switch (regm & 0x0f)
			{
			case 0x04:
				OPL3_ChannelSet4Op(chip, v);
				break;
			case 0x05:
				chip->newm = v & 0x01;
				break;
			}
		}
		else
		{
			switch (regm & 0x0f)
			{
			case 0x08:
				chip->nts = (v >> 6) & 0x01;
				break;
			}
		}
		break;
	case 0x20:
	case 0x30:
		if (ad_slot[regm & 0x1f] >= 0)
		{
			OPL3_SlotWrite20(&chip->slot[18 * high + ad_slot[regm & 0x1f]], v);
		}
		break;
	case 0x40:
	case 0x50:
		if (ad_slot[regm & 0x1f] >= 0)
		{
			OPL3_SlotWrite40(&chip->slot[18 * high + ad_slot[regm & 0x1f]], v);
		}
		break;
	case 0x60:
	case 0x70:
		if (ad_slot[regm & 0x1f] >= 0)
		{
			OPL3_SlotWrite60(&chip->slot[18 * high + ad_slot[regm & 0x1f]], v);
		}
		break;
	case 0x80:
	case 0x90:
		if (ad_slot[regm & 0x1f] >= 0)
		{
			OPL3_SlotWrite80(&chip->slot[18 * high + ad_slot[regm & 0x1f]], v);
		}
		break;
	case 0xe0:
	case 0xf0:
		if (ad_slot[regm & 0x1f] >= 0)
		{
			OPL3_SlotWriteE0(&chip->slot[18 * high + ad_slot[regm & 0x1f]], v);
		}
		break;
	case 0xa0:
		if ((regm & 0x0f) < 9)
		{
			OPL3_ChannelWriteA0(&chip->channel[9 * high + (regm & 0x0f)], v);
		}
		break;
	case 0xb0:
		if (regm == Byte<uint8_t>(0xbd) && !high)
		{
			chip->tremoloshift = (((v >> 7) ^ 1) << 1) + 2;
			chip->vibshift = ((v >> 6) & 0x01) ^ 1;
			OPL3_ChannelUpdateRhythm(chip, v);
		}
		else if ((regm & 0x0f) < 9)
		{
			OPL3_ChannelWriteB0(&chip->channel[9 * high + (regm & 0x0f)], v);
			if (v & 0x20)
			{
				OPL3_ChannelKeyOn(&chip->channel[9 * high + (regm & 0x0f)]);
			}
			else
			{
				OPL3_ChannelKeyOff(&chip->channel[9 * high + (regm & 0x0f)]);
			}
		}
		break;
	case 0xc0:
		if ((regm & 0x0f) < 9)
		{
			OPL3_ChannelWriteC0(&chip->channel[9 * high + (regm & 0x0f)], v);
		}
		break;
	}
}

void OPL3_Generate(opl3_chip *chip, Byte<int16_t> *buf)
{
	buf[1] = OPL3_ClipSample(chip->mixbuff[1]);

	for (size_t i{0}; i < 15; ++i)
	{
		OPL3_SlotCalcFB(&chip->slot[i]);
		OPL3_EnvelopeCalc(&chip->slot[i]);
		OPL3_PhaseGenerate(&chip->slot[i]);
		OPL3_SlotGenerate(&chip->slot[i]);
	}

	chip->mixbuff[0] = 0;
	for (size_t i{0}; i < 18; ++i)
	{
		Byte<int16_t> accm = 0;
		for (size_t j{0}; j < 4; ++j)
		{
			accm += *chip->channel[i].out[j];
		}
		chip->mixbuff[0] += Byte<int16_t>((accm & chip->channel[i].cha));
	}

	for (size_t i{15}; i < 18; ++i)
	{
		OPL3_SlotCalcFB(&chip->slot[i]);
		OPL3_EnvelopeCalc(&chip->slot[i]);
		OPL3_PhaseGenerate(&chip->slot[i]);
		OPL3_SlotGenerate(&chip->slot[i]);
	}

	buf[0] = OPL3_ClipSample(chip->mixbuff[0]);

	for (size_t i{18}; i < 33; ++i)
	{
		OPL3_SlotCalcFB(&chip->slot[i]);
		OPL3_EnvelopeCalc(&chip->slot[i]);
		OPL3_PhaseGenerate(&chip->slot[i]);
		OPL3_SlotGenerate(&chip->slot[i]);
	}

	chip->mixbuff[1] = 0;
	for (size_t i{0}; i < 18; ++i)
	{
		Byte<int16_t> accm = 0;
		for (size_t j{0}; j < 4; ++j)
		{
			accm += *chip->channel[i].out[j];
		}
		chip->mixbuff[1] += Byte<int16_t>(accm & chip->channel[i].chb);
	}

	for (size_t i{33}; i < 36; ++i)
	{
		OPL3_SlotCalcFB(&chip->slot[i]);
		OPL3_EnvelopeCalc(&chip->slot[i]);
		OPL3_PhaseGenerate(&chip->slot[i]);
		OPL3_SlotGenerate(&chip->slot[i]);
	}

	if ((chip->timer & 0x3f) == Byte<uint8_t>(0x3f))
	{
		chip->tremolopos = (chip->tremolopos + 1) % 210;
	}
	if (chip->tremolopos < 105)
	{
		chip->tremolo = chip->tremolopos >> chip->tremoloshift;
	}
	else
	{
		chip->tremolo = (210 - chip->tremolopos) >> chip->tremoloshift;
	}

	if ((chip->timer & 0x3ff) == 0x3ff)
	{
		chip->vibpos = (chip->vibpos + 1) & 7;
	}

	++chip->timer;

	chip->eg_add = 0;
	if (chip->eg_timer)
	{
		Byte<uint8_t> shift{0};
		while (shift < 36 && ((chip->eg_timer >> shift) & 1) == 0)
		{
			++shift;
		}
		if (shift > 12)
		{
			chip->eg_add = 0;
		}
		else
		{
			chip->eg_add = shift + 1;
		}
	}

	if (chip->eg_timerrem || chip->eg_state)
	{
		if (chip->eg_timer == 0xfffffffff)
		{
			chip->eg_timer = 0;
			chip->eg_timerrem = 1;
		}
		else
		{
			++chip->eg_timer;
			chip->eg_timerrem = 0;
		}
	}

	chip->eg_state ^= 1;

	while (chip->writebuf[chip->writebuf_cur].time <= chip->writebuf_samplecnt)
	{
		if (!(chip->writebuf[chip->writebuf_cur].reg & 0x200))
		{
			break;
		}
		chip->writebuf[chip->writebuf_cur].reg &= 0x1ff;
		OPL3_WriteReg(chip, chip->writebuf[chip->writebuf_cur].reg, chip->writebuf[chip->writebuf_cur].data);
		chip->writebuf_cur = (chip->writebuf_cur + 1) % OPL_WRITEBUF_SIZE;
	}

	++chip->writebuf_samplecnt;
}

void OPL3_GenerateResampled(opl3_chip *chip, Byte<int16_t> *buf)
{
	while (chip->samplecnt >= chip->rateratio)
	{
		chip->oldsamples[0] = chip->samples[0];
		chip->oldsamples[1] = chip->samples[1];
		OPL3_Generate(chip, chip->samples);
		chip->samplecnt -= chip->rateratio;
	}

	buf[0] = Byte<int16_t>((chip->oldsamples[0] * (chip->rateratio - chip->samplecnt) + chip->samples[0] * chip->samplecnt) / chip->rateratio);
	buf[1] = Byte<int16_t>((chip->oldsamples[1] * (chip->rateratio - chip->samplecnt) + chip->samples[1] * chip->samplecnt) / chip->rateratio);
	chip->samplecnt += 1 << RSM_FRAC;
}

void OPL3_Reset(opl3_chip *chip, Byte<uint32_t> samplerate)
{
	memset(chip, 0, sizeof(opl3_chip));
	for (size_t slotnum{0}; slotnum < 36; ++slotnum)
	{
		chip->slot[slotnum].chip = chip;
		chip->slot[slotnum].mod = &chip->zeromod;
		chip->slot[slotnum].eg_rout = 0x1ff;
		chip->slot[slotnum].eg_out = 0x1ff;
		chip->slot[slotnum].eg_gen = envelope_gen_num::release;
		chip->slot[slotnum].trem = (std::byte*)&chip->zeromod;
		chip->slot[slotnum].slot_num = slotnum;
	}

	for (size_t channum{0}; channum < 18; ++channum)
	{
		chip->channel[channum].slots[0] = &chip->slot[ch_slot[channum]];
		chip->channel[channum].slots[1] = &chip->slot[ch_slot[channum] + 3];
		chip->slot[ch_slot[channum]].channel = &chip->channel[channum];
		chip->slot[ch_slot[channum] + 3].channel = &chip->channel[channum];
		if ((channum % 9) < 3)
		{
			chip->channel[channum].pair = &chip->channel[channum + 3];
		}
		else if ((channum % 9) < 6)
		{
			chip->channel[channum].pair = &chip->channel[channum - 3];
		}
		chip->channel[channum].chip = chip;
		chip->channel[channum].out[0] = &chip->zeromod;
		chip->channel[channum].out[1] = &chip->zeromod;
		chip->channel[channum].out[2] = &chip->zeromod;
		chip->channel[channum].out[3] = &chip->zeromod;
		chip->channel[channum].chtype = ch_2op;
		chip->channel[channum].cha = 0xffff;
		chip->channel[channum].chb = 0xffff;
		chip->channel[channum].ch_num = channum;
		OPL3_ChannelSetupAlg(&chip->channel[channum]);
	}

	chip->noise = 1;
	chip->rateratio = (samplerate << RSM_FRAC) / 49716;
	chip->tremoloshift = 4;
	chip->vibshift = 1;
}

void OPL3_WriteRegBuffered(opl3_chip* chip, Byte<uint16_t> reg, Byte<uint8_t> v)
{
	if (chip->writebuf[chip->writebuf_last].reg & 0x200)
	{
		OPL3_WriteReg(chip, chip->writebuf[chip->writebuf_last].reg & Byte<uint16_t>(0x1ff), chip->writebuf[chip->writebuf_last].data);

		chip->writebuf_cur = (chip->writebuf_last + 1) % OPL_WRITEBUF_SIZE;
		chip->writebuf_samplecnt = chip->writebuf[chip->writebuf_last].time;
	}

	chip->writebuf[chip->writebuf_last].reg = reg | 0x200;
	chip->writebuf[chip->writebuf_last].data = v;
	Byte<uint64_t> time1 = chip->writebuf_lasttime + OPL_WRITEBUF_DELAY;
	Byte<uint64_t> time2 = chip->writebuf_samplecnt;

	if (time1 < time2)
	{
		time1 = time2;
	}

	chip->writebuf[chip->writebuf_last].time = time1;
	chip->writebuf_lasttime = time1;
	chip->writebuf_last = (chip->writebuf_last + 1) % OPL_WRITEBUF_SIZE;
}

void OPL3_GenerateStream(opl3_chip* chip, Byte<int16_t>* sndptr, Byte<uint32_t> numsamples)
{
	// FIXME once params fixed to be container, remove this generate_n and mixer
	::std::vector<Byte<int16_t>> mixer(numsamples);
	::std::generate_n(::std::begin(mixer), numsamples, [&sndptr](auto& iter) mutable {iter = *sndptr; sndptr += 2;});

	::std::ranges::for_each(mixer, [&chip](auto& iter){OPL3_GenerateResampled(chip, &iter);});
	/* for(size_t i{0}; i < numsamples; ++i)
	{
		OPL3_GenerateResampled(chip, sndptr);
		sndptr += 2;
	} */
}

} // end namespace cudadoom::opl
