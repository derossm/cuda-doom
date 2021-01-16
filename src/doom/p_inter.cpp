/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Handling interactions (i.e., collisions).
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

//#include <string>

#include "doomdef.h"
#include "dstrings.h"
#include "sounds.h"

#include "deh_main.h"
#include "deh_misc.h"
#include "doomstat.h"

#include "m_random.h"
#include "i_system.h"

#include "am_map.h"

#include "p_local.h"

#include "s_sound.h"

#include "p_inter.h"

constexpr size_t BONUSADD{6};

// a weapon is found with two clip loads, a big item has five clip loads
int maxammo[::std::size_t(AmmoType::NUMAMMO)] = {200, 50, 300, 50};
int clipammo[::std::size_t(AmmoType::NUMAMMO)] = {10, 4, 20, 1};

//
// GET STUFF
//

// Num is the number of clip loads, not the individual count (0= 1/2 clip).
// Returns false if the ammo can't be picked up at all.
bool P_GiveAmmo(Player* player, AmmoType ammo, int num, bool dropped) // Dropped ammo/weapons give half as much.
{
	int oldammo;

	if (ammo == AmmoType::am_noammo)
	{
		return false;
	}

	if (ammo > AmmoType::NUMAMMO)
	{
		I_Error("P_GiveAmmo: bad type %i", ammo);
	}

	if (player->ammo[::std::size_t(ammo)] == player->maxammo[::std::size_t(ammo)])
	{
		return false;
	}

	if (num)
	{
		num *= clipammo[::std::size_t(ammo)];
	}
	else
	{
		num = clipammo[::std::size_t(ammo)] / 2;
	}

	if (gameskill == SkillType::sk_baby || gameskill == SkillType::sk_nightmare)
	{
		// give double ammo in trainer mode, you'll need in nightmare
		num <<= 1;
	}

	// Halve if needed.
	if (dropped)
	{
		num >>= 1;
		// Don't round down to 0.
		if (!num)
		{
			num = 1;
		}
	}

	oldammo = player->ammo[::std::size_t(ammo)];
	player->ammo[::std::size_t(ammo)] += num;

	if (player->ammo[::std::size_t(ammo)] > player->maxammo[::std::size_t(ammo)])
	{
		player->ammo[::std::size_t(ammo)] = player->maxammo[::std::size_t(ammo)];
	}

	// If non zero ammo, don't change up weapons, player was lower on purpose.
	if (oldammo)
	{
		return true;
	}

	// We were down to zero, so select a new weapon. Preferences are not user selectable.
	switch (ammo)
	{
	case AmmoType::am_clip:
		if (player->readyweapon == WeaponType::wp_fist)
		{
			if (player->weaponowned[::std::size_t(WeaponType::wp_chaingun)])
			{
				player->pendingweapon = WeaponType::wp_chaingun;
			}
			else
			{
				player->pendingweapon = WeaponType::wp_pistol;
			}
		}
		break;

	case AmmoType::am_shell:
		if (player->readyweapon == WeaponType::wp_fist || player->readyweapon == WeaponType::wp_pistol)
		{
			if (player->weaponowned[::std::size_t(WeaponType::wp_shotgun)])
			{
				player->pendingweapon = WeaponType::wp_shotgun;
			}
		}
		break;

	case AmmoType::am_cell:
		if (player->readyweapon == WeaponType::wp_fist || player->readyweapon == WeaponType::wp_pistol)
		{
			if (player->weaponowned[::std::size_t(WeaponType::wp_plasma)])
			{
				player->pendingweapon = WeaponType::wp_plasma;
			}
		}
		break;

	case AmmoType::am_misl:
		if (player->readyweapon == WeaponType::wp_fist)
		{
			if (player->weaponowned[::std::size_t(WeaponType::wp_missile)])
			{
				player->pendingweapon = WeaponType::wp_missile;
			}
		}
		break;

	default:
		break;
	}

	return true;
}

// show weapon pickup messages in multiplayer games
::std::string const WeaponPickupMessages[::std::size_t(WeaponType::NUMWEAPONS)] =
{
	NULL, // wp_fist
	NULL, // wp_pistol
	GOTSHOTGUN,
	GOTCHAINGUN,
	GOTLAUNCHER,
	GOTPLASMA,
	GOTBFG9000,
	GOTCHAINSAW,
	GOTSHOTGUN2
};

// The weapon name may have a mobjflag_e::MF_DROPPED flag ored in.
bool P_GiveWeapon(Player* player, WeaponType weapon, bool dropped)
{
	bool gaveammo;
	bool gaveweapon;

	if (netgame && (deathmatch != 2) && !dropped)
	{
		// leave placed weapons forever on net games
		if (player->weaponowned[::std::size_t(weapon)])
		{
			return false;
		}

		player->bonuscount += BONUSADD;
		player->weaponowned[::std::size_t(weapon)] = true;

		if (deathmatch)
		{
			P_GiveAmmo(player, weaponinfo[::std::size_t(weapon)].ammo, 5, false);
		}
		else
		{
			P_GiveAmmo(player, weaponinfo[::std::size_t(weapon)].ammo, 2, false);
		}

		player->pendingweapon = weapon;
		// show weapon pickup messages in multiplayer games
		player->message = DEH_String(WeaponPickupMessages[::std::size_t(weapon)]);

		if (player == &players[consoleplayer])
		{
			S_StartSound(nullptr, sfxenum_t::sfx_wpnup);
		}

		return false;
	}

	if (weaponinfo[::std::size_t(weapon)].ammo != AmmoType::am_noammo)
	{
		// give one clip with a dropped weapon, two clips with a found weapon
		// Just need to pass that it's dropped.
		gaveammo = P_GiveAmmo(player, weaponinfo[::std::size_t(weapon)].ammo, 2, dropped);
	}
	else
	{
		gaveammo = false;
	}

	if (player->weaponowned[::std::size_t(weapon)])
	{
		gaveweapon = false;
	}
	else
	{
		gaveweapon = true;
		player->weaponowned[::std::size_t(weapon)] = true;
		player->pendingweapon = weapon;
	}

	return (gaveweapon || gaveammo);
}

// Returns false if the body isn't needed at all
bool P_GiveBody(Player* player, int num)
{
	if (player->health >= MAXHEALTH)
	{
		return false;
	}

	// TODO verify num is > 0 and that health + num doesn't overflow
	player->health += num;

	if (player->health > MAXHEALTH)
	{
		player->health = MAXHEALTH;
	}

	player->health = player->health;

	return true;
}

// Returns false if the armor is worse than the current armor.
bool P_GiveArmor(Player* player, int armortype)
{
	int hits;

	hits = armortype * 100;
	if (player->armorpoints >= hits)
	{
		return false;	// don't pick up
	}

	player->armortype = armortype;
	player->armorpoints = hits;

	return true;
}

void P_GiveCard(Player* player, CardType card)
{
	if (player->cards[::std::size_t(card)])
	{
		return;
	}

	player->bonuscount += netgame ? BONUSADD : 0; // Fix "Key pickup resets palette"
	player->cards[::std::size_t(card)] = 1;
}

bool P_GivePower(Player* player, PowerType_t power)
{
	if (power == PowerType_t::pw_invulnerability)
	{
		player->powers[::std::size_t(power)] = PowerDuration_t::INVULNTICS;
		return true;
	}

	if (power == PowerType_t::pw_invisibility)
	{
		player->powers[::std::size_t(power)] = PowerDuration_t::INVISTICS;
		player->flags |= mobjflag_e::MF_SHADOW;
		return true;
	}

	if (power == PowerType_t::pw_infrared)
	{
		player->powers[::std::size_t(power)] = PowerDuration_t::INFRATICS;
		return true;
	}

	if (power == PowerType_t::pw_ironfeet)
	{
		player->powers[::std::size_t(power)] = PowerDuration_t::IRONTICS;
		return true;
	}

	if (power == PowerType_t::pw_strength)
	{
		P_GiveBody(player, 100);
		player->powers[::std::size_t(power)] = 1;
		return true;
	}

	if (player->powers[::std::size_t(power)])
	{
		return false;	// already got it
	}

	player->powers[::std::size_t(power)] = 1;
	return true;
}

void P_TouchSpecialThing(MapObject* special, MapObject* toucher)
{
	Player* player;
	int i;
	fixed_t delta;
	sfxenum_t sound;
	const bool dropped = (((int)special->flags & (int)mobjflag_e::MF_DROPPED) != 0);

	delta = special->z - toucher->z;

	if (delta > toucher->height || delta < -8 * FRACUNIT)
	{
		// out of reach
		return;
	}

	sound = sfxenum_t::sfx_itemup;
	player = toucher->player;

	// Dead thing touching.
	// Can happen with a sliding player corpse.
	if (toucher->health <= 0)
	{
		return;
	}

	// Identify by sprite.
	switch (special->sprite)
	{
		// armor
	case spritenum_t::SPR_ARM1:
		if (!P_GiveArmor(player, deh_green_armor_class))
		{
			return;
		}
		player->message = DEH_String(GOTARMOR);
		break;

	case spritenum_t::SPR_ARM2:
		if (!P_GiveArmor(player, deh_blue_armor_class))
		{
			return;
		}
		player->message = DEH_String(GOTMEGA);
		break;

		// bonus items
	case spritenum_t::SPR_BON1:
		player->health++;		// can go over 100%
		if (player->health > deh_max_health)
		{
			player->health = deh_max_health;
		}
		player->health = player->health;
		player->message = DEH_String(GOTHTHBONUS);
		break;

	case spritenum_t::SPR_BON2:
		player->armorpoints++;		// can go over 100%
		if (player->armorpoints > deh_max_armor && gameversion > GameVersion::exe_doom_1_2)
		{
			player->armorpoints = deh_max_armor;
		}
		// deh_green_armor_class only applies to the green armor shirt;
		// for the armor helmets, armortype 1 is always used.
		if (!player->armortype)
		{
			player->armortype = 1;
		}
		player->message = DEH_String(GOTARMBONUS);
		break;

	case spritenum_t::SPR_SOUL:
		player->health += deh_soulsphere_health;
		if (player->health > deh_max_soulsphere)
		{
			player->health = deh_max_soulsphere;
		}
		player->health = player->health;
		player->message = DEH_String(GOTSUPER);
		if (gameversion > GameVersion::exe_doom_1_2)
		{
			sound = sfxenum_t::sfx_getpow;
		}
		break;

	case spritenum_t::SPR_MEGA:
		if (gamemode != GameMode::commercial)
		{
			return;
		}
		player->health = deh_megasphere_health;
		player->health = player->health;
		// We always give armor type 2 for the megasphere; dehacked only
		// affects the MegaArmor.
		P_GiveArmor(player, 2);
		player->message = DEH_String(GOTMSPHERE);
		if (gameversion > GameVersion::exe_doom_1_2)
		{
			sound = sfxenum_t::sfx_getpow;
		}
		break;

		// cards
		// leave cards for everyone
	case spritenum_t::SPR_BKEY:
		if (!player->cards[::std::size_t(CardType::it_bluecard)])
		{
			player->message = DEH_String(GOTBLUECARD);
		}
		P_GiveCard(player, CardType::it_bluecard);
		if (!netgame)
		{
			break;
		}
		return;

	case spritenum_t::SPR_YKEY:
		if (!player->cards[::std::size_t(CardType::it_yellowcard)])
		{
			player->message = DEH_String(GOTYELWCARD);
		}
		P_GiveCard(player, CardType::it_yellowcard);
		if (!netgame)
		{
			break;
		}
		return;

	case spritenum_t::SPR_RKEY:
		if (!player->cards[::std::size_t(CardType::it_redcard)])
		{
			player->message = DEH_String(GOTREDCARD);
		}
		P_GiveCard(player, CardType::it_redcard);
		if (!netgame)
		{
			break;
		}
		return;

	case spritenum_t::SPR_BSKU:
		if (!player->cards[::std::size_t(CardType::it_blueskull)])
		{
			player->message = DEH_String(GOTBLUESKUL);
		}
		P_GiveCard(player, CardType::it_blueskull);
		if (!netgame)
		{
			break;
		}
		return;

	case spritenum_t::SPR_YSKU:
		if (!player->cards[::std::size_t(CardType::it_yellowskull)])
		{
			player->message = DEH_String(GOTYELWSKUL);
		}
		P_GiveCard(player, CardType::it_yellowskull);
		if (!netgame)
		{
			break;
		}
		return;

	case spritenum_t::SPR_RSKU:
		if (!player->cards[::std::size_t(CardType::it_redskull)])
		{
			player->message = DEH_String(GOTREDSKULL);
		}
		P_GiveCard(player, CardType::it_redskull);
		if (!netgame)
		{
			break;
		}
		return;

		// medikits, heals
	case spritenum_t::SPR_STIM:
		if (!P_GiveBody(player, 10))
		{
			return;
		}
		player->message = DEH_String(GOTSTIM);
		break;

	case spritenum_t::SPR_MEDI:
		if (!P_GiveBody(player, 25))
		{
			return;
		}

		// show "Picked up a Medikit that you really need" message as intended
		if (player->health < 50)
		{
			player->message = DEH_String(GOTMEDINEED);
		}
		else
		{
			player->message = DEH_String(GOTMEDIKIT);
		}

		break;

		// power ups
	case spritenum_t::SPR_PINV:
		if (!P_GivePower(player, PowerType_t::pw_invulnerability))
		{
			return;
		}
		player->message = DEH_String(GOTINVUL);
		if (gameversion > GameVersion::exe_doom_1_2)
		{
			sound = sfxenum_t::sfx_getpow;
		}
		break;

	case spritenum_t::SPR_PSTR:
		if (!P_GivePower(player, PowerType_t::pw_strength))
		{
			return;
		}
		player->message = DEH_String(GOTBERSERK);
		if (player->readyweapon != WeaponType::wp_fist)
		{
			player->pendingweapon = WeaponType::wp_fist;
		}
		if (gameversion > GameVersion::exe_doom_1_2)
		{
			sound = sfxenum_t::sfx_getpow;
		}
		break;

	case spritenum_t::SPR_PINS:
		if (!P_GivePower(player, PowerType_t::pw_invisibility))
		{
			return;
		}
		player->message = DEH_String(GOTINVIS);
		if (gameversion > GameVersion::exe_doom_1_2)
		{
			sound = sfxenum_t::sfx_getpow;
		}
		break;

	case spritenum_t::SPR_SUIT:
		if (!P_GivePower(player, PowerType_t::pw_ironfeet))
		{
			return;
		}
		player->message = DEH_String(GOTSUIT);
		if (gameversion > GameVersion::exe_doom_1_2)
		{
			sound = sfxenum_t::sfx_getpow;
		}
		break;

	case spritenum_t::SPR_PMAP:
		if (!P_GivePower(player, PowerType_t::pw_allmap))
		{
			return;
		}
		player->message = DEH_String(GOTMAP);
		if (gameversion > GameVersion::exe_doom_1_2)
		{
			sound = sfxenum_t::sfx_getpow;
		}
		break;

	case spritenum_t::SPR_PVIS:
		if (!P_GivePower(player, PowerType_t::pw_infrared))
		{
			return;
		}
		player->message = DEH_String(GOTVISOR);
		if (gameversion > GameVersion::exe_doom_1_2)
		{
			sound = sfxenum_t::sfx_getpow;
		}
		break;

		// ammo
		// Give half ammo for drops of all types.
	case spritenum_t::SPR_CLIP:
		/*
		if (special->flags & mobjflag_e::MF_DROPPED)
		{
			if (!P_GiveAmmo(player,am_clip,0))
			return;
		}
		else
		{
			if (!P_GiveAmmo(player,am_clip,1))
			return;
		}
		*/
		if (!P_GiveAmmo(player, AmmoType::am_clip, 1, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTCLIP);
		break;

	case spritenum_t::SPR_AMMO:
		if (!P_GiveAmmo(player, AmmoType::am_clip, 5, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTCLIPBOX);
		break;

	case spritenum_t::SPR_ROCK:
		if (!P_GiveAmmo(player, AmmoType::am_misl, 1, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTROCKET);
		break;

	case spritenum_t::SPR_BROK:
		if (!P_GiveAmmo(player, AmmoType::am_misl, 5, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTROCKBOX);
		break;

	case spritenum_t::SPR_CELL:
		if (!P_GiveAmmo(player, AmmoType::am_cell, 1, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTCELL);
		break;

	case spritenum_t::SPR_CELP:
		if (!P_GiveAmmo(player, AmmoType::am_cell, 5, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTCELLBOX);
		break;

	case spritenum_t::SPR_SHEL:
		if (!P_GiveAmmo(player, AmmoType::am_shell, 1, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTSHELLS);
		break;

	case spritenum_t::SPR_SBOX:
		if (!P_GiveAmmo(player, AmmoType::am_shell, 5, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTSHELLBOX);
		break;

	case spritenum_t::SPR_BPAK:
		if (!player->backpack)
		{
			for (i = 0; i < ::std::size_t(AmmoType::NUMAMMO); ++i)
			{
				player->maxammo[i] *= 2;
			}
			player->backpack = true;
		}
		for (i = 0; i < ::std::size_t(AmmoType::NUMAMMO); ++i)
		{
			P_GiveAmmo(player, i, 1, false);
		}
		player->message = DEH_String(GOTBACKPACK);
		break;

		// weapons
		// Give half ammo for all dropped weapons.
	case spritenum_t::SPR_BFUG:
		if (!P_GiveWeapon(player, WeaponType::wp_bfg, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTBFG9000);
		sound = sfxenum_t::sfx_wpnup;
		break;

	case spritenum_t::SPR_MGUN:
		if (!P_GiveWeapon(player, WeaponType::wp_chaingun, (special->flags & mobjflag_e::MF_DROPPED) != 0))
		{
			return;
		}
		player->message = DEH_String(GOTCHAINGUN);
		sound = sfxenum_t::sfx_wpnup;
		break;

	case spritenum_t::SPR_CSAW:
		if (!P_GiveWeapon(player, WeaponType::wp_chainsaw, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTCHAINSAW);
		sound = sfxenum_t::sfx_wpnup;
		break;

	case spritenum_t::SPR_LAUN:
		if (!P_GiveWeapon(player, WeaponType::wp_missile, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTLAUNCHER);
		sound = sfxenum_t::sfx_wpnup;
		break;

	case spritenum_t::SPR_PLAS:
		if (!P_GiveWeapon(player, WeaponType::wp_plasma, dropped))
		{
			return;
		}
		player->message = DEH_String(GOTPLASMA);
		sound = sfxenum_t::sfx_wpnup;
		break;

	case spritenum_t::SPR_SHOT:
		if (!P_GiveWeapon(player, WeaponType::wp_shotgun, (special->flags & mobjflag_e::MF_DROPPED) != 0))
		{
			return;
		}
		player->message = DEH_String(GOTSHOTGUN);
		sound = sfxenum_t::sfx_wpnup;
		break;

	case spritenum_t::SPR_SGN2:
		if (!P_GiveWeapon(player, WeaponType::wp_supershotgun, (special->flags & mobjflag_e::MF_DROPPED) != 0))
		{
			return;
		}
		player->message = DEH_String(GOTSHOTGUN2);
		sound = sfxenum_t::sfx_wpnup;
		break;

	default:
		I_Error("P_SpecialThing: Unknown gettable thing");
	}

	if (special->flags & mobjflag_e::MF_COUNTITEM)
	{
		player->itemcount++;
	}
	P_RemoveMobj(special);
	player->bonuscount += BONUSADD;
	if (player == &players[consoleplayer])
	{
		S_StartSound(nullptr, sound);
	}
}

void P_KillMobj(MapObject* source, MapObject* target)
{
	mobjtype_t item;
	MapObject* mo;

	target->flags &= ~(mobjflag_e::MF_SHOOTABLE | mobjflag_e::MF_FLOAT | mobjflag_e::MF_SKULLFLY);

	if (target->type != mobjtype_t::MT_SKULL)
	{
		target->flags &= ~mobjflag_e::MF_NOGRAVITY;
	}

	target->flags |= mobjflag_e::MF_CORPSE | mobjflag_e::MF_DROPOFF;
	target->height >>= 2;

	if (source && source->player)
	{
		// count for intermission
		if (target->flags & mobjflag_e::MF_COUNTKILL)
		{
			source->player->killcount++;
		}

		if (target->player)
		{
			source->player->frags[target->player - players]++;
		}
	}
	else if (!netgame && (target->flags & mobjflag_e::MF_COUNTKILL))
	{
		// count all monster deaths, even those caused by other monsters
		players[0].killcount++;
	}

	if (target->player)
	{
		// count environment kills against you
		if (!source)
		{
			target->player->frags[target->player - players]++;
		}

		target->flags &= ~mobjflag_e::MF_SOLID;
		target->player->playerstate = PlayerState::dead;
		P_DropWeapon(target->player);
		// center view when dying
		target->player->centering = true;
		// Reset the yellow bonus palette when the player dies
		target->player->bonuscount = 0;
		// Remove the effect of the inverted palette when the player dies
		target->player->fixedcolormap = target->player->powers[::std::size_t(PowerType_t::pw_infrared)] ? 1 : 0;

		if (target->player == &players[consoleplayer] && automapactive)
		{
			// don't die in auto map, switch view prior to dying
			AM_Stop();
		}
	}

	// Lost Soul, Pain Elemental and Barrel explosions are translucent
	if (target->type == mobjtype_t::MT_SKULL || target->type == mobjtype_t::MT_PAIN || target->type == mobjtype_t::MT_BARREL)
	{
		target->flags |= mobjflag_e::MF_TRANSLUCENT;
	}

	if (target->health < -target->info->spawnhealth && target->info->xdeathstate)
	{
		P_SetMobjState(target, target->info->xdeathstate);
	}
	else
	{
		P_SetMobjState(target, target->info->deathstate);
	}

	target->tics -= P_Random() & 3;

	// randomly flip corpse, blood and death animation sprites
	if (target->flags & mobjflag_e::MF_FLIPPABLE)
	{
		target->health = (target->health & (int)~1) - (Crispy_Random() & 1);
	}

	if (target->tics < 1)
	{
		target->tics = 1;
	}

	//	I_StartSound(&actor->r, actor->info->deathsound);

	// In Chex Quest, monsters don't drop items.

	if (gameversion == GameVersion::exe_chex)
	{
		return;
	}

	// Drop stuff.
	// This determines the kind of object spawned
	// during the death frame of a thing.
	if (target->info->droppeditem != mobjtype_t::MT_NULL) // drop generalization
	{
		item = target->info->droppeditem;
	}
	else
	{
		return;
	}

	mo = P_SpawnMobj(target->x, target->y, ONFLOORZ, item);
	mo->flags |= mobjflag_e::MF_DROPPED;	// special versions of items
}

// P_DamageMobj
// Damages both enemies and players
// "inflictor" is the thing that caused the damage
// creature or missile, can be NULL (slime, etc)
// "source" is the thing to target after taking damage
// creature or NULL
// Source and inflictor are the same for melee attacks.
// Source can be NULL for slime, barrel explosions
// and other environmental stuff.
void P_DamageMobj(MapObject* target, MapObject* inflictor, MapObject* source, int damage)
{
	unsigned ang;
	int saved;
	Player* player;
	fixed_t thrust;
	int temp;

	if (!(target->flags & mobjflag_e::MF_SHOOTABLE))
	{
		return;	// shouldn't happen...
	}

	if (target->health <= 0)
	{
		return;
	}

	if (target->flags & mobjflag_e::MF_SKULLFLY)
	{
		target->momx = target->momy = target->momz = 0;
	}

	player = target->player;
	if (player && gameskill == SkillType::sk_baby)
	{
		damage >>= 1;	// take half damage in trainer mode
	}

	// Some close combat weapons should not
	// inflict thrust and push the victim out of reach,
	// thus kick away unless using the chainsaw.
	if (inflictor && !(target->flags & mobjflag_e::MF_NOCLIP) && (!source || !source->player || source->player->readyweapon != WeaponType::wp_chainsaw))
	{
		ang = R_PointToAngle2(inflictor->x, inflictor->y, target->x, target->y);
		thrust = damage * (FRACUNIT >> 3) * 100 / target->info->mass;

		// make fall forwards sometimes
		if (damage < 40 && damage > target->health && target->z - inflictor->z > 64 * FRACUNIT && (P_Random() & 1))
		{
			ang += ANG180;
			thrust *= 4;
		}

		ang >>= ANGLETOFINESHIFT;
		target->momx += FixedMul(thrust, finecosine[ang]);
		target->momy += FixedMul(thrust, finesine[ang]);
	}

	// player specific
	if (player)
	{
		// end of game hell hack
		if (target->subsector->sector->special == 11 && damage >= target->health)
		{
			damage = target->health - 1;
		}

		// Below certain threshold,
		// ignore damage in GOD mode, or with INVUL power.
		if (damage < 1000 && ((player->cheats & CheatType::CF_GODMODE) || player->powers[::std::size_t(PowerType_t::pw_invulnerability)]))
		{
			return;
		}

		if (player->armortype)
		{
			if (player->armortype == 1)
			{
				saved = damage / 3;
			}
			else
			{
				saved = damage / 2;
			}

			if (player->armorpoints <= saved)
			{
				// armor is used up
				saved = player->armorpoints;
				player->armortype = 0;
			}

			player->armorpoints -= saved;
			damage -= saved;
		}

		player->health -= damage;	// mirror mobj health here for Dave
		// negative player health
		player->neghealth = player->health;
		if (player->neghealth < -99)
		{
			player->neghealth = -99;
		}

		if (player->health < 0)
		{
			player->health = 0;
		}

		player->attacker = source;
		player->damagecount += damage;	// add damage after armor / invuln

		if (player->damagecount > 100)
		{
			player->damagecount = 100;	// teleport stomp does 10k points...
		}

		temp = damage < 100 ? damage : 100;

		if (player == &players[consoleplayer])
		{
			I_Tactile(40, 10, 40 + temp * 2);
		}
	}

	// do the damage
	target->health -= damage;
	if (target->health <= 0)
	{
		P_KillMobj(source, target);
		return;
	}

	if ((P_Random() < target->info->painchance) && !(target->flags & mobjflag_e::MF_SKULLFLY))
	{
		target->flags |= mobjflag_e::MF_JUSTHIT;	// fight back!
		P_SetMobjState(target, target->info->painstate);
	}

	target->reactiontime = 0;		// we're awake now...

	if ((!target->threshold || target->type == mobjtype_t::MT_VILE) && source && (source != target || gameversion <= GameVersion::exe_doom_1_2)
		&& source->type != mobjtype_t::MT_VILE)
	{
		// if not intent on another player,
		// chase after this one
		target->target = source;
		target->threshold = BASETHRESHOLD;
		if (target->state == &states[::std::size_t(target->info->spawnstate)] && target->info->seestate != statenum_t::S_NULL)
		{
			P_SetMobjState(target, target->info->seestate);
		}
	}
}
