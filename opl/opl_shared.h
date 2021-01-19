/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

namespace cudadoom::opl
{

//class SlotBase;
class ChipBase;
class ChannelBase;

class Slot;
class Chip;
class Channel;

constexpr size_t OPL_WRITEBUF_SIZE{1024};
constexpr size_t OPL_WRITEBUF_DELAY{2};

constexpr size_t RSM_FRAC{10};

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

class ChannelBase
{
public:
	Byte<uint16_t> f_num; // chip, channel, slot
	Byte<uint16_t> cha; // chip, channel
	Byte<uint16_t> chb; // chip, channel
	Byte<uint8_t> block; // chip, channel, slot
	Byte<uint8_t> fb; // chip, channel, slot
	Byte<uint8_t> ch_num; // chip, channel
	Byte<uint8_t> chtype; // chip, channel
	Byte<uint8_t> ksv; // channel, slot
	Byte<uint8_t> con; // channel
	Byte<uint8_t> alg; // channel

public:
	virtual void ChannelSetupAlg() = 0;
	virtual void ChannelWriteA0(Byte<uint8_t> data) = 0;
	virtual void ChannelWriteB0(Byte<uint8_t> data) = 0;
	virtual void ChannelWriteC0(Byte<uint8_t> data) = 0;
	virtual void ChannelKeyOn() = 0;
	virtual void ChannelKeyOff() = 0;
	virtual ~ChannelBase() = default;

};

class ChipBase
{
public:
	Byte<uint32_t> noise; // chip
	Byte<int16_t> zeromod; // chip, channel, slot
	Byte<uint16_t> timer; // chip, slot
	Byte<uint8_t> rhy; // chip
	Byte<uint8_t> vibpos; // chip
	Byte<uint8_t> vibshift; // chip
	Byte<uint8_t> rm_hh_bit2; // chip
	Byte<uint8_t> rm_hh_bit3; // chip
	Byte<uint8_t> rm_hh_bit7; // chip
	Byte<uint8_t> rm_hh_bit8; // chip
	Byte<uint8_t> rm_tc_bit3; // chip
	Byte<uint8_t> rm_tc_bit5; // chip
	Byte<uint8_t> newm; // chip, channel, slot
	Byte<uint8_t> nts; // chip, channel
	Byte<uint8_t> eg_state; // chip, slot
	Byte<uint8_t> eg_add; // chip, slot
	Byte<uint8_t> tremolo; // chip, slot

public:
	virtual ~ChipBase() = default;
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
typedef void(*envelope_genfunc)(Slot* slot);

class Envelope
{

};
static Byte<int16_t> OPL3_EnvelopeCalcExp(Byte<uint32_t> level)
{
	if (level > 0x1fff)
	{
		level = 0x1fff;
	}

	return static_cast<Byte<int16_t>>((exprom[level & 0xff] << 1) >> (level >> 8));
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

	return Byte<int16_t>(Byte<uint16_t>(OPL3_EnvelopeCalcExp(Byte<uint32_t>(out + (envelope << 3)))) ^ neg);
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

	return Byte<int16_t>(Byte<uint16_t>(OPL3_EnvelopeCalcExp(Byte<uint32_t>(out + (envelope << 3)))) ^ neg);
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

	return Byte<int16_t>(OPL3_EnvelopeCalcExp(envelope << 3) ^ neg);
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

	return Byte<int16_t>(Byte<uint16_t>(OPL3_EnvelopeCalcExp(Byte<uint32_t>(out + (envelope << 3)))) ^ neg);
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

} // end namespace cudadoom::opl
