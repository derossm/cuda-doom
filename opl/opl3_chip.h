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
#include "opl3_channel.h"

namespace cudadoom::opl
{

class Chip : public ChipBase
{
private:
	std::array<Channel, 18> channel;
	std::array<Slot, 36> slot;

	uint64_t writebuf_samplecnt; // chip
	size_t writebuf_cur; // chip
	size_t writebuf_last; // chip
	TimeType writebuf_lasttime; // chip
	Byte<TimeType> eg_timer; // chip

	std::array<Byte<int32_t>, 2> mixbuff; // chip
	std::array<Byte<int16_t>, 2> samples; // chip
	std::array<Byte<int16_t>, 2> oldsamples; // chip

	Byte<int32_t> rateratio; // chip
	Byte<int32_t> samplecnt; // chip
	Byte<uint8_t> eg_timerrem; // chip

	Byte<uint8_t> tremolopos; // chip
	Byte<uint8_t> tremoloshift; // chip

	//OPL3L
	std::array<opl3_writebuf, OPL_WRITEBUF_SIZE> writebuf; // chip

public:

	Byte<int16_t> ClipSample(Byte<int32_t> sample)
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

	void WriteReg(Byte<uint16_t> reg, Byte<uint8_t> v)
	{
		Byte<uint8_t> high{Byte<uint8_t>((reg >> 8) & 0x01)};
		Byte<uint8_t> regm{Byte<uint8_t>(reg & 0xff)};
		switch (regm & 0xf0)
		{
		case 0x00:
			if (high)
			{
				switch (regm & 0x0f)
				{
				case 0x04:
					ChannelSet4Op(v);
					break;
				case 0x05:
					newm = v & 0x01;
					break;
				}
			}
			else
			{
				switch (regm & 0x0f)
				{
				case 0x08:
					nts = (v >> 6) & 0x01;
					break;
				}
			}
			break;
		case 0x20:
		case 0x30:
			if (ad_slot[regm & 0x1f] >= 0)
			{
				slot[18 * high + ad_slot[regm & 0x1f]].SlotWrite20(v);
			}
			break;
		case 0x40:
		case 0x50:
			if (ad_slot[regm & 0x1f] >= 0)
			{
				slot[18 * high + ad_slot[regm & 0x1f]].SlotWrite40(v);
			}
			break;
		case 0x60:
		case 0x70:
			if (ad_slot[regm & 0x1f] >= 0)
			{
				slot[18 * high + ad_slot[regm & 0x1f]].SlotWrite60(v);
			}
			break;
		case 0x80:
		case 0x90:
			if (ad_slot[regm & 0x1f] >= 0)
			{
				slot[18 * high + ad_slot[regm & 0x1f]].SlotWrite80(v);
			}
			break;
		case 0xe0:
		case 0xf0:
			if (ad_slot[regm & 0x1f] >= 0)
			{
				slot[18 * high + ad_slot[regm & 0x1f]].SlotWriteE0(v);
			}
			break;
		case 0xa0:
			if ((regm & 0x0f) < 9)
			{
				channel[9 * high + (regm & 0x0f)].ChannelWriteA0(v);
			}
			break;
		case 0xb0:
			if (regm == Byte<uint8_t>(0xbd) && !high)
			{
				tremoloshift = (((v >> 7) ^ 1) << 1) + 2;
				vibshift = ((v >> 6) & 0x01) ^ 1;
				ChannelUpdateRhythm(v);
			}
			else if ((regm & 0x0f) < 9)
			{
				channel[9 * high + (regm & 0x0f)].ChannelWriteB0(v);
				if (v & 0x20)
				{
					channel[9 * high + (regm & 0x0f)].ChannelKeyOn();
				}
				else
				{
					channel[9 * high + (regm & 0x0f)].ChannelKeyOff();
				}
			}
			break;
		case 0xc0:
			if ((regm & 0x0f) < 9)
			{
				channel[9 * high + (regm & 0x0f)].ChannelWriteC0(v);
			}
			break;
		}
	}

	std::array<Byte<int16_t>, 2> Generate()
	{
		std::array<Byte<int16_t>, 2> buf;
		buf[1] = ClipSample(mixbuff[1]);

		for (size_t i{0}; i < 15; ++i)
		{
			slot[i].SlotCalcFB();
			slot[i].EnvelopeCalc();
			slot[i].PhaseGenerate(this);
			slot[i].SlotGenerate();
		}

		mixbuff[0] = 0;
		for (size_t i{0}; i < 18; ++i)
		{
			Byte<int16_t> accm = 0;
			for (size_t j{0}; j < 4; ++j)
			{
				accm += *channel[i].channel_out[j];
			}
			mixbuff[0] += Byte<int16_t>((accm & channel[i].cha));
		}

		for (size_t i{15}; i < 18; ++i)
		{
			slot[i].SlotCalcFB();
			slot[i].EnvelopeCalc();
			slot[i].PhaseGenerate(this);
			slot[i].SlotGenerate();
		}

		buf[0] = ClipSample(mixbuff[0]);

		for (size_t i{18}; i < 33; ++i)
		{
			slot[i].SlotCalcFB();
			slot[i].EnvelopeCalc();
			slot[i].PhaseGenerate(this);
			slot[i].SlotGenerate();
		}

		mixbuff[1] = 0;
		for (size_t i{0}; i < 18; ++i)
		{
			Byte<int16_t> accm = 0;
			for (size_t j{0}; j < 4; ++j)
			{
				accm += *channel[i].channel_out[j];
			}
			mixbuff[1] += Byte<int16_t>(accm & channel[i].chb);
		}

		for (size_t i{33}; i < 36; ++i)
		{
			slot[i].SlotCalcFB();
			slot[i].EnvelopeCalc();
			slot[i].PhaseGenerate(this);
			slot[i].SlotGenerate();
		}

		if ((timer & 0x3f) == Byte<uint8_t>(0x3f))
		{
			tremolopos = (tremolopos + 1) % 210;
		}
		if (tremolopos < 105)
		{
			tremolo = tremolopos >> tremoloshift;
		}
		else
		{
			tremolo = (210 - tremolopos) >> tremoloshift;
		}

		if ((timer & 0x3ff) == 0x3ff)
		{
			vibpos = (vibpos + 1) & 7;
		}

		++timer;

		eg_add = 0;
		if (eg_timer)
		{
			Byte<uint8_t> shift{0};
			while (shift < 36 && ((eg_timer >> shift) & 1) == 0)
			{
				++shift;
			}
			if (shift > 12)
			{
				eg_add = 0;
			}
			else
			{
				eg_add = shift + 1;
			}
		}

		if (eg_timerrem || eg_state)
		{
			if (eg_timer == 0xfffffffff)
			{
				eg_timer = 0;
				eg_timerrem = 1;
			}
			else
			{
				++eg_timer;
				eg_timerrem = 0;
			}
		}

		eg_state ^= 1;

		while (writebuf[writebuf_cur].time <= writebuf_samplecnt)
		{
			if (!(writebuf[writebuf_cur].reg & 0x200))
			{
				break;
			}
			writebuf[writebuf_cur].reg &= 0x1ff;
			WriteReg(writebuf[writebuf_cur].reg, writebuf[writebuf_cur].data);
			writebuf_cur = (writebuf_cur + 1) % OPL_WRITEBUF_SIZE;
		}

		++writebuf_samplecnt;
	}

	int16_t* GenerateResampled()
	{
		int16_t buf[2];
		while (samplecnt >= rateratio)
		{
			oldsamples[0] = samples[0];
			oldsamples[1] = samples[1];
			samples = Generate();
			samplecnt -= rateratio;
		}

		buf[0] = int16_t((oldsamples[0] * (rateratio - samplecnt) + samples[0] * samplecnt) / rateratio);
		buf[1] = int16_t((oldsamples[1] * (rateratio - samplecnt) + samples[1] * samplecnt) / rateratio);
		samplecnt += 1 << RSM_FRAC;
		return buf;
	}

	void Reset(Byte<uint32_t> samplerate)
	{
		//memset(chip, 0, sizeof(opl3_chip));
		// FIXME reset the object
		for (size_t slotnum{0}; slotnum < 36; ++slotnum)
		{
			//slot[slotnum].chip = chip;
			slot[slotnum].slot_mod = &zeromod;
			slot[slotnum].eg_rout = 0x1ff;
			slot[slotnum].eg_out = 0x1ff;
			slot[slotnum].eg_gen = envelope_gen_num::release;
			//slot[slotnum].trem = &zeromod;
			slot[slotnum].slot_num = slotnum;
		}

		for (size_t channum{0}; channum < 18; ++channum)
		{
			channel[channum].slots[0] = &slot[ch_slot[channum]];
			channel[channum].slots[1] = &slot[ch_slot[channum] + 3];
			slot[ch_slot[channum]].channel = &channel[channum];
			slot[ch_slot[channum] + 3].channel = &channel[channum];
			if ((channum % 9) < 3)
			{
				channel[channum].pair = &channel[channum + 3];
			}
			else if ((channum % 9) < 6)
			{
				channel[channum].pair = &channel[channum - 3];
			}
			//channel[channum].chip = chip;
			channel[channum].channel_out[0] = &zeromod;
			channel[channum].channel_out[1] = &zeromod;
			channel[channum].channel_out[2] = &zeromod;
			channel[channum].channel_out[3] = &zeromod;
			channel[channum].chtype = ch_2op;
			channel[channum].cha = 0xffff;
			channel[channum].chb = 0xffff;
			channel[channum].ch_num = channum;
			channel[channum].ChannelSetupAlg();
		}

		noise = 1;
		rateratio = (samplerate << RSM_FRAC) / 49716;
		tremoloshift = 4;
		vibshift = 1;
	}

	void WriteRegBuffered(Byte<uint16_t> reg, Byte<uint8_t> v)
	{
		if (writebuf[writebuf_last].reg & 0x200)
		{
			WriteReg(writebuf[writebuf_last].reg & Byte<uint16_t>(0x1ff), writebuf[writebuf_last].data); // FIXME

			writebuf_cur = (writebuf_last + 1) % OPL_WRITEBUF_SIZE;
			writebuf_samplecnt = writebuf[writebuf_last].time;
		}

		writebuf[writebuf_last].reg = reg | 0x200;
		writebuf[writebuf_last].data = v;
		Byte<uint64_t> time1 = writebuf_lasttime + OPL_WRITEBUF_DELAY;
		Byte<uint64_t> time2 = writebuf_samplecnt;

		if (time1 < time2)
		{
			time1 = time2;
		}

		writebuf[writebuf_last].time = time1;
		writebuf_lasttime = time1;
		writebuf_last = (writebuf_last + 1) % OPL_WRITEBUF_SIZE;
	}

	auto GenerateStream(uint64_t numsamples)
	{
		::std::vector<uint32_t> mixer(numsamples);
		::std::generate_n(::std::begin(mixer), numsamples, [&]() mutable { return *reinterpret_cast<uint32_t*>(GenerateResampled()); });

		return reinterpret_cast<uint8_t*>(mixer.data());
		/* for(size_t i{0}; i < numsamples; ++i)
		{
			GenerateResampled(chip, sndptr);
			sndptr += 2;
		} */
	}

	void ChannelUpdateRhythm(Byte<uint8_t> data)
	{
		rhy = data & 0x3f;
		if (rhy & 0x20)
		{
			Channel* channel6 = &channel[6];
			Channel* channel7 = &channel[7];
			Channel* channel8 = &channel[8];
			channel6->channel_out[0] = &channel6->slots[1]->slot_out;
			channel6->channel_out[1] = &channel6->slots[1]->slot_out;
			channel6->channel_out[2] = &zeromod;
			channel6->channel_out[3] = &zeromod;
			channel7->channel_out[0] = &channel7->slots[0]->slot_out;
			channel7->channel_out[1] = &channel7->slots[0]->slot_out;
			channel7->channel_out[2] = &channel7->slots[1]->slot_out;
			channel7->channel_out[3] = &channel7->slots[1]->slot_out;
			channel8->channel_out[0] = &channel8->slots[0]->slot_out;
			channel8->channel_out[1] = &channel8->slots[0]->slot_out;
			channel8->channel_out[2] = &channel8->slots[1]->slot_out;
			channel8->channel_out[3] = &channel8->slots[1]->slot_out;
			for (size_t chnum{6}; chnum < 9; ++chnum)
			{
				channel[chnum].chtype = ch_drum;
			}
			channel6->ChannelSetupAlg();
			channel7->ChannelSetupAlg();
			channel8->ChannelSetupAlg();
			//hh
			if (rhy & 0x01)
			{
				channel7->slots[0]->EnvelopeKeyOn(egk_drum);
			}
			else
			{
				channel7->slots[0]->EnvelopeKeyOff(egk_drum);
			}
			//tc
			if (rhy & 0x02)
			{
				channel8->slots[1]->EnvelopeKeyOn(egk_drum);
			}
			else
			{
				channel8->slots[1]->EnvelopeKeyOff(egk_drum);
			}
			//tom
			if (rhy & 0x04)
			{
				channel8->slots[0]->EnvelopeKeyOn(egk_drum);
			}
			else
			{
				channel8->slots[0]->EnvelopeKeyOff(egk_drum);
			}
			//sd
			if (rhy & 0x08)
			{
				channel7->slots[1]->EnvelopeKeyOn(egk_drum);
			}
			else
			{
				channel7->slots[1]->EnvelopeKeyOff(egk_drum);
			}
			//bd
			if (rhy & 0x10)
			{
				channel6->slots[0]->EnvelopeKeyOn(egk_drum);
				channel6->slots[1]->EnvelopeKeyOn(egk_drum);
			}
			else
			{
				channel6->slots[0]->EnvelopeKeyOff(egk_drum);
				channel6->slots[1]->EnvelopeKeyOff(egk_drum);
			}
		}
		else
		{
			for (size_t chnum{6}; chnum < 9; ++chnum)
			{
				channel[chnum].chtype = ch_2op;
				channel[chnum].ChannelSetupAlg();
				channel[chnum].slots[0]->EnvelopeKeyOff(egk_drum);
				channel[chnum].slots[1]->EnvelopeKeyOff(egk_drum);
			}
		}
	}

	void ChannelSet4Op(Byte<uint8_t> data)
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
				channel[chnum].chtype = ch_4op;
				channel[chnum + 3].chtype = ch_4op2;
			}
			else
			{
				channel[chnum].chtype = ch_2op;
				channel[chnum + 3].chtype = ch_2op;
			}
		}
	}

};

} // end namespace cudadoom::opl
