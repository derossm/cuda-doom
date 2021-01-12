/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/

#include "bytetype.h"
#include "common.h"

#include <cstdlib>

#define CATCH_CONFIG_MAIN
#pragma warning(push, 0)
#include "catch.h"
#pragma warning(pop)

//int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//{
//	return Catch::Session().run();
//}

TEST_CASE("Byte: wrapper for std::byte", "[ByteType]")
{
	Byte<uint8_t> b_u8type;
	Byte<uint16_t> b_u16type;
	Byte<uint32_t> b_u32type;
	Byte<uint64_t> b_u64type;

	Byte<int8_t> b_s8type;
	Byte<int16_t> b_s16type;
	Byte<int32_t> b_s32type;
	Byte<int64_t> b_s64type;

	int i{0xFF};
	unsigned u{0xFF};
	short s{0xFF};
	unsigned short us{0xFF};
	long l{0xFF};
	unsigned long ul{0xFF};

	::std::byte _byte{0xFF};

	auto k = b_u8type & i;
	auto k0 = b_u8type | i;
	auto k1 = b_u8type ^ i;
	auto k2 = b_u8type + i;
	auto k3 = b_u8type - i;
	auto k4 = b_u8type * i;
	auto k5 = b_u8type / i;
	auto k6 = b_u8type % i;
	auto k7 = ~b_u8type;

	int t[5];
	t[b_u8type] = 0;

	REQUIRE(k == (0 & 0xFF));
	REQUIRE(k0 == (0 | 0xFF));
	REQUIRE(k1 == (0 ^ 0xFF));
	REQUIRE(k2 == (0 + 0xFF));
	REQUIRE(k3 == (0 - 0xFF));
	REQUIRE(k4 == (0 * 0xFF));
	REQUIRE(k5 == (0 / 0xFF));
	REQUIRE(k6 == (0 % 0xFF));
	REQUIRE(k7 == ~0x00);
	//REQUIRE(k8 == (0 | 0xFF));
	//REQUIRE(k9 == (0 | 0xFF));
}
