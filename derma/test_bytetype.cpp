/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#include "stdafx.h"

#include "common.h"
#include "bytetype.h"

#define CATCH_CONFIG_MAIN
#pragma warning(push, 0)
#include "catch.h"
#pragma warning(pop)

//int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//{
//	return Catch::Session().run();
//}

TEMPLATE_TEST_CASE("Byte: wrapper for std::byte; non-assignment operators", "[Byte][template]", uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t )
{
	Byte<TestType> b_u8type;

	constexpr size_t i{0xFFFFFFFFFFFFFFFF};

	//int t[5];
	//t[b_u8type] = 0;

	Byte<TestType> k0 = b_u8type & i;
	Byte<TestType> k1 = b_u8type | i;
	Byte<TestType> k2 = b_u8type ^ i;
	Byte<TestType> k3 = b_u8type + i;
	Byte<TestType> k4 = b_u8type - i;
	Byte<TestType> k5 = b_u8type * i;
	Byte<TestType> k6 = b_u8type / i;
	Byte<TestType> k7 = b_u8type % i;
	Byte<TestType> k8 = ~b_u8type;

	REQUIRE(k0 == static_cast<TestType>(0 & 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k1 == static_cast<TestType>(0 | 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k2 == static_cast<TestType>(0 ^ 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k3 == static_cast<TestType>(0 + 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k4 == static_cast<TestType>(0 - 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k5 == static_cast<TestType>(0 * 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k6 == static_cast<TestType>(0 / 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k7 == static_cast<TestType>(0 % 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k8 == static_cast<TestType>(~0));
}

TEMPLATE_TEST_CASE("Byte: wrapper for std::byte; assignment operators", "[Byte][template]", uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t )
{
	Byte<TestType> b_u8type;

	constexpr size_t i{0xFFFFFFFFFFFFFFFF};

	//int t[5];
	//t[b_u8type] = 0;

	Byte<TestType> k0 = b_u8type;
	Byte<TestType> k1 = b_u8type;
	Byte<TestType> k2 = b_u8type;
	Byte<TestType> k3 = b_u8type;
	Byte<TestType> k4 = b_u8type;
	Byte<TestType> k5 = b_u8type;
	Byte<TestType> k6 = b_u8type;
	Byte<TestType> k7 = b_u8type;
	Byte<TestType> k8 = b_u8type;
	Byte<TestType> k9 = 17;
	Byte<TestType> k10 = true;

	k0 &= i;
	k1 |= i;
	k2 ^= i;
	k3 += i;
	k4 -= i;
	k5 *= i;
	k6 /= i;
	k7 %= i;
 	k8 = ~b_u8type;
	k9 = -k9;
	k10 = !k10;

	REQUIRE(k0 == static_cast<TestType>(0 & 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k1 == static_cast<TestType>(0 | 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k2 == static_cast<TestType>(0 ^ 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k3 == static_cast<TestType>(0 + 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k4 == static_cast<TestType>(0 - 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k5 == static_cast<TestType>(0 * 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k6 == static_cast<TestType>(0 / 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k7 == static_cast<TestType>(0 % 0xFFFFFFFFFFFFFFFF));
	REQUIRE(k8 == static_cast<TestType>(~0));
	REQUIRE(-k9 == static_cast<TestType>(0 - k9));
	REQUIRE(!k10 == true);
}
