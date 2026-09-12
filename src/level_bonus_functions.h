#pragma once
#include "entity.h"
#include <array>

namespace Game {
	extern const std::array< Weapon, 6>* defaultWeapons;
}

void LevelBonus_40(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	—крыта€ атака:
	+1 к урону если
	ловкость персонажа
	выше ловкости цели
	*/

	if (attacker->dexterity > victim->dexterity) {
		initialDamage += 1;
	}	
}

void LevelBonus_41(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	ѕорыв к действию:
	¬ первый ход наносит
	двойной урон
	оружием
	*/
	if (attacker->step == 1) {
		uint8_t wpnIdx = attacker->wpnIdx;
		initialDamage += (*Game::defaultWeapons)[wpnIdx].damage;
	}
}

void LevelBonus_42(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	ярость:
	+2 к урону в первые 3
	хода, потом -1 к урону
	*/
	if (attacker->step < 4) initialDamage += 3;
	else initialDamage -= 1;
}

void LevelBonus_43(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*	Ћовкость +1	*/
	attacker->dexterity += 1;
}

void LevelBonus_44(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	ўит:
	-3 к получаемому
	урону если сила
	персонажа выше
	силы атакующего
	*/

	if (victim->strength > attacker->strength ) {
		initialDamage -= 3;
	}
}

void LevelBonus_45(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	 аменна€ кожа:
	ѕолучаемый урон
	снижаетс€ на
	значение
	выносливости
	*/

	initialDamage -= victim->endurance;
}

void LevelBonus_46(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	яд:
	Ќаносит
	дополнительные +1
	урона на втором
	ходу, +2 на третьем
	и так далее.
	*/
	if (attacker->step > 1) {
		initialDamage += attacker->step - 1;
	}
}

void LevelBonus_47(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/* —ила +1 */
	attacker->strength += 1;
}

void LevelBonus_48(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/* ¬ыносливость +1 */
	attacker->endurance += 1;
}

void LevelBonus_49(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	ѕолучает вдвое
	больше урона, если
	его бьют дроб€щим
	оружием
	*/

	uint8_t wpnIdx = attacker->wpnIdx;
	DamageType dmgTp = (*Game::defaultWeapons)[wpnIdx].dmgType;

	if (dmgTp == DamageType::Crushing) initialDamage += initialDamage;
}

void LevelBonus_50(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	–уб€щее оружие не
	наносит ему урона
	(но урон от силы и
	прочих
	особенностей, даже
	"порыва к действию"
	воина, работает)
	*/

	uint8_t wpnIdx = attacker->wpnIdx;
	DamageType dmgTp = (*Game::defaultWeapons)[wpnIdx].dmgType;
	uint8_t damage = (*Game::defaultWeapons)[wpnIdx].damage;

	if (dmgTp == DamageType::Chopping) initialDamage -= damage;
}

void LevelBonus_51(uint8_t& initialDamage, Entity* attacker, Entity* victim) {
	/*
	 аждый 3-й ход
	дышит огнЄм,
	нанос€
	дополнительно 3
	урона
	*/

	uint8_t modStep = attacker->step % 3;
	if (modStep == 0) initialDamage += 3;
}
