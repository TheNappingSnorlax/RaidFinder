/*
 * This file is part of RaidFinder
 * Copyright (C) 2019 by Admiral_Fish
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "RaidGenerator.hpp"
#include <Core/RNG/XoroShiro.hpp>

constexpr u8 toxtricityAmpedNatures[13] = { 3, 4, 2, 8, 9, 19, 22, 11, 13, 14, 0, 6, 24 };

static inline u16 getSv(u32 val)
{
    return ((val >> 16) ^ (val & 0xFFFF)) >> 4;
}

static inline u8 getShinyType(u32 sidtid, u32 pid)
{
    u16 val = (sidtid ^ pid) >> 16;
    if ((val ^ (sidtid & 0xffff)) == (pid & 0xffff))
    {
        return 2; // Square shiny
    }
    else
    {
        return 1; // Star shiny
    }
}

RaidGenerator::RaidGenerator(u32 startFrame, u32 maxResults, u8 abilityType, u16 tid, u16 sid, u8 genderType,
                             u8 genderRatio, u8 ivCount, u16 species) :
    startFrame(startFrame), maxResults(maxResults), abilityType(abilityType), tid(tid), sid(sid), ivCount(ivCount), genderType(genderType), genderRatio(genderRatio), species(species)
{
}

QVector<Frame> RaidGenerator::generate(const FrameCompare &compare, u64 seed) const
{
    QVector<Frame> frames;
    u16 tsv = (tid ^ sid) >> 4;

    for (u32 i = 1; i < startFrame; i++)
    {
        seed += 0x82A2B175229D6A5B;
    }

    for (u32 frame = 0; frame < maxResults; frame++, seed += 0x82A2B175229D6A5B)
    {
        XoroShiro rng(seed);
        Frame result(seed, startFrame + frame);

        u32 ec = rng.nextInt(0xffffffff, 0xffffffff);
        result.setEC(ec);

        u32 sidtid = rng.nextInt(0xffffffff, 0xffffffff);
        u32 pid = rng.nextInt(0xffffffff, 0xffffffff);

        u16 ftsv = getSv(sidtid);
        u16 psv = getSv(pid);

        // Game uses a fake TID/SID to determine shiny or not
        // PID is later modified using the actual TID/SID of trainer if necessary
        if (ftsv == psv) // Force shiny
        {
            u8 shinyType = getShinyType(sidtid, pid);
            result.setShiny(shinyType);
            if (psv != tsv)
            {
                u16 high = (pid & 0xFFFF) ^ tid ^ sid ^ (shinyType == 1);
                pid = (high << 16) | (pid & 0xFFFF);
            }
        }
        else // Force non shiny
        {
            result.setShiny(0);
            if (psv == tsv)
            {
                pid ^= 0x10000000;
            }
        }
        result.setPID(pid);

        // Set IVs that will be 31s
        for (u8 i = 0; i < ivCount;)
        {
            u8 index = static_cast<u8>(rng.nextInt(6, 7));
            if (result.getIV(index) == 255)
            {
                result.setIV(index, 31);
                i++;
            }
        }

        // Fill rest of IVs with rand calls
        for (u8 i = 0; i < 6; i++)
        {
            if (result.getIV(i) == 255)
            {
                result.setIV(i, static_cast<u8>(rng.nextInt(32, 31)));
            }
        }

        if (abilityType == 4) // Allow hidden ability
        {
            result.setAbility(static_cast<u8>(rng.nextInt(3, 3)));
        }
        else if (abilityType == 3) // No hidden ability
        {
            result.setAbility(static_cast<u8>(rng.nextInt(2, 1)));
        }

        // Altform, doesn't seem to have a rand call for raids

        if (genderType == 0) // Random
        {
            if (genderRatio == 255) // Locked genderless
            {
                result.setGender(2);
            }
            else if (genderRatio == 254) // Locked female
            {
                result.setGender(1);
            }
            else if (genderRatio == 0) // Locked male
            {
                result.setGender(0);
            }
            else // Random
            {
                result.setGender(static_cast<u8>(rng.nextInt(253, 255) + 1) < genderRatio);
            }
        }
        else if (genderType == 1) // Male
        {
            result.setGender(0);
        }
        else if (genderType == 2) // Female
        {
            result.setGender(1);
        }
        else if (genderType == 3) // Genderless
        {
            result.setGender(2);
        }

        if (species != 849)
        {
            result.setNature(static_cast<u8>(rng.nextInt(25, 31)));
        }
        else
        {
            result.setNature(toxtricityAmpedNatures[rng.nextInt(13, 15)]);
        }

        if (compare.compareFrame(result))
        {
            frames.append(result);
        }
    }

    return frames;
}
