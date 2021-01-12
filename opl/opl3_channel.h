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
#include "opl3_slot.h"

namespace cudadoom::opl
{

class Channel : public ChannelBase
{
	/*std::array<Byte<int16_t>*, 4> out;

	std::array<Slot*, 2> slots;
	ChipBase* chip;

	Channel* pair;

	Byte<uint8_t> chtype;
	Byte<uint16_t> f_num;
	Byte<uint8_t> block;
	Byte<uint8_t> fb;
	Byte<uint8_t> con;
	Byte<uint8_t> alg;
	Byte<uint8_t> ksv;
	Byte<uint16_t> cha;
	Byte<uint16_t> chb;
	Byte<uint8_t> ch_num;*/

public:

	void ChannelSetupAlg()
	{
		if (chtype == ch_drum)
		{
			if (ch_num == 7 || ch_num == 8)
			{
				slots[0]->mod = &chip->zeromod;
				slots[1]->mod = &chip->zeromod;
				return;
			}
			switch (alg & 0x01)
			{
			case 0x00:
				slots[0]->mod = &slots[0]->fbmod;
				slots[1]->mod = &slots[0]->out;
				break;
			case 0x01:
				slots[0]->mod = &slots[0]->fbmod;
				slots[1]->mod = &chip->zeromod;
				break;
			}
			return;
		}
		if (alg & 0x08)
		{
			return;
		}
		if (alg & 0x04)
		{
			pair->out[0] = &chip->zeromod;
			pair->out[1] = &chip->zeromod;
			pair->out[2] = &chip->zeromod;
			pair->out[3] = &chip->zeromod;
			switch (alg & 0x03)
			{
			case 0x00:
				pair->slots[0]->mod = &pair->slots[0]->fbmod;
				pair->slots[1]->mod = &pair->slots[0]->out;
				slots[0]->mod = &pair->slots[1]->out;
				slots[1]->mod = &slots[0]->out;
				out[0] = &slots[1]->out;
				out[1] = &chip->zeromod;
				out[2] = &chip->zeromod;
				out[3] = &chip->zeromod;
				break;
			case 0x01:
				pair->slots[0]->mod = &pair->slots[0]->fbmod;
				pair->slots[1]->mod = &pair->slots[0]->out;
				slots[0]->mod = &chip->zeromod;
				slots[1]->mod = &slots[0]->out;
				out[0] = &pair->slots[1]->out;
				out[1] = &slots[1]->out;
				out[2] = &chip->zeromod;
				out[3] = &chip->zeromod;
				break;
			case 0x02:
				pair->slots[0]->mod = &pair->slots[0]->fbmod;
				pair->slots[1]->mod = &chip->zeromod;
				slots[0]->mod = &pair->slots[1]->out;
				slots[1]->mod = &slots[0]->out;
				out[0] = &pair->slots[0]->out;
				out[1] = &slots[1]->out;
				out[2] = &chip->zeromod;
				out[3] = &chip->zeromod;
				break;
			case 0x03:
				pair->slots[0]->mod = &pair->slots[0]->fbmod;
				pair->slots[1]->mod = &chip->zeromod;
				slots[0]->mod = &pair->slots[1]->out;
				slots[1]->mod = &chip->zeromod;
				out[0] = &pair->slots[0]->out;
				out[1] = &slots[0]->out;
				out[2] = &slots[1]->out;
				out[3] = &chip->zeromod;
				break;
			}
		}
		else
		{
			switch (alg & 0x01)
			{
			case 0x00:
				slots[0]->mod = &slots[0]->fbmod;
				slots[1]->mod = &slots[0]->out;
				out[0] = &slots[1]->out;
				out[1] = &chip->zeromod;
				out[2] = &chip->zeromod;
				out[3] = &chip->zeromod;
				break;
			case 0x01:
				slots[0]->mod = &slots[0]->fbmod;
				slots[1]->mod = &chip->zeromod;
				out[0] = &slots[0]->out;
				out[1] = &slots[1]->out;
				out[2] = &chip->zeromod;
				out[3] = &chip->zeromod;
				break;
			}
		}
	}

	void ChannelWriteA0(Byte<uint8_t> data)
	{
		if (chip->newm && chtype == ch_4op2)
		{
			return;
		}
		f_num = (f_num & 0x300) | data;
		ksv = (block << 1) | ((f_num >> (Byte<uint8_t>(0x09) - chip->nts)) & 0x01);
		slots[0]->EnvelopeUpdateKSL();
		slots[1]->EnvelopeUpdateKSL();
		if (chip->newm && chtype == ch_4op)
		{
			pair->f_num = f_num;
			pair->ksv = ksv;
			pair->slots[0]->EnvelopeUpdateKSL();
			pair->slots[1]->EnvelopeUpdateKSL();
		}
	}

	void ChannelWriteB0(Byte<uint8_t> data)
	{
		if (chip->newm && chtype == ch_4op2)
		{
			return;
		}
		f_num = (f_num & 0xff) | ((data & 0x03) << 8);
		block = (data >> 2) & 0x07;
		ksv = (block << 1) | ((f_num >> (Byte<uint8_t>(0x09) - chip->nts)) & 0x01);
		slots[0]->EnvelopeUpdateKSL();
		slots[1]->EnvelopeUpdateKSL();
		if (chip->newm && chtype == ch_4op)
		{
			pair->f_num = f_num;
			pair->block = block;
			pair->ksv = ksv;
			pair->slots[0]->EnvelopeUpdateKSL();
			pair->slots[1]->EnvelopeUpdateKSL();
		}
	}

	void ChannelWriteC0(Byte<uint8_t> data)
	{
		fb = (data & 0x0e) >> 1;
		con = data & 0x01;
		alg = con;
		if (chip->newm)
		{
			if (chtype == ch_4op)
			{
				pair->alg = 0x04 | (con << 1) | (pair->con);
				alg = 0x08;
				pair->ChannelSetupAlg();
			}
			else if (chtype == ch_4op2)
			{
				alg = 0x04 | (pair->con << 1) | (con);
				pair->alg = 0x08;
				ChannelSetupAlg();
			}
			else
			{
				ChannelSetupAlg();
			}
		}
		else
		{
			ChannelSetupAlg();
		}
		if (chip->newm)
		{
			cha = ((data >> 4) & 0x01) ? ~0 : 0;
			chb = ((data >> 5) & 0x01) ? ~0 : 0;
		}
		else
		{
			cha = chb = static_cast<Byte<uint16_t>>(~0);
		}
	}

	void ChannelKeyOn()
	{
		if (chip->newm)
		{
			if (chtype == ch_4op)
			{
				slots[0]->EnvelopeKeyOn(egk_norm);
				slots[1]->EnvelopeKeyOn(egk_norm);
				pair->slots[0]->EnvelopeKeyOn(egk_norm);
				pair->slots[1]->EnvelopeKeyOn(egk_norm);
			}
			else if (chtype == ch_2op || chtype == ch_drum)
			{
				slots[0]->EnvelopeKeyOn(egk_norm);
				slots[1]->EnvelopeKeyOn(egk_norm);
			}
		}
		else
		{
			slots[0]->EnvelopeKeyOn(egk_norm);
			slots[1]->EnvelopeKeyOn(egk_norm);
		}
	}

	void ChannelKeyOff()
	{
		if (chip->newm)
		{
			if (chtype == ch_4op)
			{
				slots[0]->EnvelopeKeyOff(egk_norm);
				slots[1]->EnvelopeKeyOff(egk_norm);
				pair->slots[0]->EnvelopeKeyOff(egk_norm);
				pair->slots[1]->EnvelopeKeyOff(egk_norm);
			}
			else if (chtype == ch_2op || chtype == ch_drum)
			{
				slots[0]->EnvelopeKeyOff(egk_norm);
				slots[1]->EnvelopeKeyOff(egk_norm);
			}
		}
		else
		{
			slots[0]->EnvelopeKeyOff(egk_norm);
			slots[1]->EnvelopeKeyOff(egk_norm);
		}
	}
};

} // end namespace cudadoom::opl
