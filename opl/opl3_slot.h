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
public:
	ChannelBase* channel{};
	ChipBase* chip{};

	Byte<int16_t>* mod{};
	Byte<uint8_t>* trem{};

	Byte<uint32_t> pg_reset;
	Byte<uint32_t> pg_phase;
	Byte<uint16_t> pg_phase_out;

	Byte<int16_t> out;
	Byte<int16_t> fbmod;
	Byte<int16_t> prout;

	Byte<int16_t> eg_rout;
	Byte<int16_t> eg_out;
	Byte<uint8_t> eg_inc;
	Byte<uint8_t> eg_rate;
	Byte<uint8_t> eg_ksl;

	Byte<uint8_t> reg_vib;
	Byte<uint8_t> reg_type;
	Byte<uint8_t> reg_ksr;
	Byte<uint8_t> reg_mult;
	Byte<uint8_t> reg_ksl;
	Byte<uint8_t> reg_tl;
	Byte<uint8_t> reg_ar;
	Byte<uint8_t> reg_dr;
	Byte<uint8_t> reg_sl;
	Byte<uint8_t> reg_rr;
	Byte<uint8_t> reg_wf;

	Byte<uint8_t> key;
	Byte<uint8_t> slot_num;

	envelope_gen_num eg_gen;
public:
	//opl3_slot()

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
		eg_out = eg_rout + (reg_tl << 2) + (eg_ksl >> kslshift[reg_ksl]) + *trem;
		if (key && eg_gen == envelope_gen_num::release)
		{
			reset = 1;
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
		ks = channel->ksv >> ((reg_ksr ^ 1) << 1);
		nonzero = (reg_rate != 0);
		rate = ks + (reg_rate << 2);
		rate_hi = rate >> 2;
		rate_lo = rate & 0x03;
		if (rate_hi & 0x10)
		{
			rate_hi = 0x0f;
		}
		eg_shift = rate_hi + chip->eg_add;
		shift = 0;
		if (nonzero)
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
		eg_rout = eg_rout;
		eg_inc = 0;
		eg_off = 0;
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
			else if (key && shift > 0 && rate_hi != Byte<uint8_t>(0x0f))
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
		if (!key)
		{
			eg_gen = envelope_gen_num::release;
		}
	}

	void EnvelopeKeyOn(Byte<uint8_t> type)
	{
		key |= type;
	}

	void EnvelopeKeyOff(Byte<uint8_t> type)
	{
		key &= ~type;
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
		out = envelope_sin[reg_wf](Byte<uint16_t>(pg_phase_out + *mod), Byte<uint16_t>(eg_out));
	}

	void SlotCalcFB()
	{
		if (channel->fb != Byte<uint8_t>(0x00))
		{
			fbmod = (prout + out) >> (Byte<uint8_t>(0x09) - channel->fb);
		}
		else
		{
			fbmod = 0;
		}
		prout = out;
	}
};

} // end namespace cudadoom::opl
