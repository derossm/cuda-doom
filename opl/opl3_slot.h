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

#include "opl_shared.h"

namespace cudadoom::opl
{

class Slot// : public SlotBase
{
private:

	Byte<int16_t> prout; // local
	Byte<uint8_t> eg_ksl; // local
	Byte<uint8_t> reg_type; // local
	Byte<uint8_t> reg_ksr; // local
	Byte<uint8_t> reg_ksl; // local
	Byte<uint8_t> reg_tl; // local
	Byte<uint8_t> reg_ar; // local
	Byte<uint8_t> reg_dr; // local
	Byte<uint8_t> reg_sl; // local
	Byte<uint8_t> reg_rr; // local
	Byte<uint8_t> reg_wf; // local

	bool slot_key; // local

public:
	ChannelBase* channel{nullptr};
	ChipBase* chip{nullptr};

	Byte<uint8_t>* trem{nullptr}; // chip
	Byte<int16_t>* slot_mod{nullptr}; // chip, channel

	Byte<uint32_t> pg_phase; // chip

	Byte<int16_t> slot_out; // chip, channel
	Byte<uint16_t> pg_phase_out; // chip

	Byte<int16_t> fbmod; // channel
	Byte<int16_t> eg_rout; // chip
	Byte<int16_t> eg_out; // chip
	Byte<uint8_t> slot_num; // chip
	Byte<uint8_t> reg_vib; // chip
	Byte<uint8_t> reg_mult; // chip

	envelope_gen_num eg_gen; // chip
	bool pg_reset; // chip

public:
	// Phase Generator
	void PhaseGenerate(ChipBase* _chip)
	{
		Byte<uint16_t> f_num;
		Byte<uint32_t> basefreq;
		Byte<uint8_t> rm_xor;
		Byte<uint8_t> n_bit;
		Byte<uint32_t> noise;
		Byte<uint16_t> phase;

		chip = _chip;
		f_num = channel->f_num;
		if (reg_vib)
		{
			Byte<int8_t> range;
			Byte<uint8_t> vibpos;

			range = (f_num >> 7) & 7;
			vibpos = chip->vibpos;

			if (!(vibpos & 3))
			{
				range = 0;
			}
			else if (vibpos & 1)
			{
				range >>= 1;
			}
			range >>= chip->vibshift;

			if (vibpos & 4)
			{
				range = -range;
			}
			f_num += range;
		}
		basefreq = (f_num << channel->block) >> 1;
		phase = (Byte<uint16_t>)(pg_phase >> 9);
		if (pg_reset)
		{
			pg_phase = 0;
		}
		pg_phase += (basefreq * mt[reg_mult]) >> 1;
		// Rhythm mode
		noise = chip->noise;
		pg_phase_out = phase;
		if (slot_num == 13) // hh
		{
			chip->rm_hh_bit2 = (phase >> 2) & 1;
			chip->rm_hh_bit3 = (phase >> 3) & 1;
			chip->rm_hh_bit7 = (phase >> 7) & 1;
			chip->rm_hh_bit8 = (phase >> 8) & 1;
		}
		if (slot_num == 17 && (chip->rhy & 0x20)) // tc
		{
			chip->rm_tc_bit3 = (phase >> 3) & 1;
			chip->rm_tc_bit5 = (phase >> 5) & 1;
		}
		if (chip->rhy & 0x20)
		{
			rm_xor = (chip->rm_hh_bit2 ^ chip->rm_hh_bit7)
					| (chip->rm_hh_bit3 ^ chip->rm_tc_bit5)
					| (chip->rm_tc_bit3 ^ chip->rm_tc_bit5);
			switch (slot_num)
			{
			case 13: // hh
				pg_phase_out = rm_xor << 9;
				if (rm_xor ^ (noise & 1))
				{
					pg_phase_out |= 0xd0;
				}
				else
				{
					pg_phase_out |= 0x34;
				}
				break;
			case 16: // sd
				pg_phase_out = (chip->rm_hh_bit8 << 9) | ((chip->rm_hh_bit8 ^ (noise & 1)) << 8);
				break;
			case 17: // tc
				pg_phase_out = (rm_xor << 9) | 0x80;
				break;
			default:
				break;
			}
		}
		n_bit = ((noise >> 14) ^ noise) & 0x01;
		chip->noise = (noise >> 1) | (n_bit << 22);
	}

	void EnvelopeUpdateKSL()
	{
		Byte<int16_t> ksl = Byte<uint8_t,1>((kslrom[channel->f_num >> 6] << 2)) - ((0x08 - channel->block) << 5);
		if (ksl < 0)
		{
			ksl = 0;
		}
		eg_ksl = static_cast<uint8_t>(ksl.bits);
	}

	void EnvelopeCalc()
	{
		eg_out = eg_rout + (reg_tl << 2) + (eg_ksl >> kslshift[reg_ksl]) + *trem;

		bool reset{false};
		Byte<uint8_t> reg_rate{0};
		if (slot_key && eg_gen == envelope_gen_num::release)
		{
			reset = true;
			reg_rate = reg_ar;
		}
		else
		{
			switch (eg_gen)
			{
			case envelope_gen_num::attack:
				reg_rate = reg_ar;
				break;
			case envelope_gen_num::decay:
				reg_rate = reg_dr;
				break;
			case envelope_gen_num::sustain:
				if (!reg_type)
				{
					reg_rate = reg_rr;
				}
				break;
			case envelope_gen_num::release:
				reg_rate = reg_rr;
				break;
			}
		}

		pg_reset = reset;

		// temp convenience aliases for initializing `rate_hi` and `rate_lo`
		Byte<uint8_t> ks{channel->ksv >> ((reg_ksr ^ 1) << 1)};
		Byte<uint8_t> rate{ks + (reg_rate << 2)};

		Byte<uint8_t> rate_hi{rate >> 2};
		Byte<uint8_t> rate_lo{rate & 0x03};
	
		if (rate_hi & 0x10)
		{
			rate_hi = 0x0f;
		}

		Byte<uint8_t> eg_shift{rate_hi + chip->eg_add};
		Byte<uint8_t> shift{0};
		if (reg_rate != 0)
		{
			if (rate_hi < 12)
			{
				if (chip->eg_state)
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
				shift = (rate_hi & 0x03) + eg_incstep[rate_lo][chip->timer & 0x03];
				if (shift & 0x04)
				{
					shift = 0x03;
				}
				if (!shift)
				{
					shift = chip->eg_state;
				}
			}
		}
		//eg_rout = eg_rout;
		Byte<int16_t> eg_inc = 0;
		Byte<uint8_t> eg_off = 0;
		// Instant attack
		if (reset && rate_hi == Byte<uint8_t>(0x0f))
		{
			eg_rout = 0x00;
		}
		// Envelope off
		if ((eg_rout & 0x1f8) == 0x1f8)
		{
			eg_off = 1;
		}
		if (eg_gen != envelope_gen_num::attack && !reset && eg_off)
		{
			eg_rout = 0x1ff;
		}
		switch (eg_gen)
		{
		case envelope_gen_num::attack:
			if (!eg_rout)
			{
				eg_gen = envelope_gen_num::decay;
			}
			else if (slot_key && shift > 0 && rate_hi != Byte<uint8_t>(0x0f))
			{
				eg_inc = ((~eg_rout) << shift) >> 4;
			}
			break;
		case envelope_gen_num::decay:
			if ((eg_rout >> 4) == reg_sl)
			{
				eg_gen = envelope_gen_num::sustain;
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
		eg_rout = (eg_rout + eg_inc) & 0x1ff;
		// Key off
		if (reset)
		{
			eg_gen = envelope_gen_num::attack;
		}
		if (!slot_key)
		{
			eg_gen = envelope_gen_num::release;
		}
	}

	void EnvelopeKeyOn(Byte<uint8_t> type)
	{
		slot_key |= type;
	}

	void EnvelopeKeyOff(Byte<uint8_t> type)
	{
		slot_key &= ~type;
	}

	// Slot
	void SlotWrite20(Byte<uint8_t> data)
	{
		if ((data >> 7) & 0x01)
		{
			trem = &chip->tremolo;
		}
		else
		{
			// FIXME
			//trem = &chip->zeromod;
			*trem = chip->zeromod;
		}
		reg_vib = (data >> 6) & 0x01;
		reg_type = (data >> 5) & 0x01;
		reg_ksr = (data >> 4) & 0x01;
		reg_mult = data & 0x0f;
	}

	void SlotWrite40(Byte<uint8_t> data)
	{
		reg_ksl = (data >> 6) & 0x03;
		reg_tl = data & 0x3f;
		EnvelopeUpdateKSL();
	}

	void SlotWrite60(Byte<uint8_t> data)
	{
		reg_ar = (data >> 4) & 0x0f;
		reg_dr = data & 0x0f;
	}

	void SlotWrite80(Byte<uint8_t> data)
	{
		reg_sl = (data >> 4) & 0x0f;
		if (reg_sl == Byte<uint8_t>(0x0f))
		{
			reg_sl = 0x1f;
		}
		reg_rr = data & 0x0f;
	}

	void SlotWriteE0(Byte<uint8_t> data)
	{
		reg_wf = data & 0x07;
		if (chip->newm == Byte<uint8_t>(0x00))
		{
			reg_wf &= 0x03;
		}
	}

	void SlotGenerate()
	{
		slot_out = envelope_sin[reg_wf](Byte<uint16_t>(pg_phase_out + *slot_mod), Byte<uint16_t>(eg_out));
	}

	void SlotCalcFB()
	{
		if (channel->fb != Byte<uint8_t>(0x00))
		{
			fbmod = (prout + slot_out) >> (Byte<uint8_t>(0x09) - channel->fb);
		}
		else
		{
			fbmod = 0;
		}
		prout = slot_out;
	}
};

} // end namespace cudadoom::opl
