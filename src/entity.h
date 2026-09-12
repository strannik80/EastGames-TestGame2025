#pragma once
#include <stdint.h>

// all weapon and damage types
enum class  WeaponType : uint8_t {
	Sword = 0,
	Club,
	Dagger,
	Axe,
	Spear,
	LegendarySword,
	NOT_SET
};

enum class  DamageType : uint8_t {
	Chopping = 0,
	Crushing,
	Piercing,
	NOT_SET
};

struct Weapon {
	WeaponType wpnType = WeaponType::NOT_SET;
	DamageType dmgType = DamageType::NOT_SET;
	uint8_t damage = 0;
	uint8_t txtIdx = 0;
};

enum class  EntityType : uint8_t {
	Character = 0,
	Monster,
	NOT_SET
};


struct Entity {
	EntityType entType = EntityType::NOT_SET;

	uint8_t wpnIdx = 0;
	uint8_t health = 0;
	uint8_t step = 1;
	uint8_t charLevel = 0;
	uint8_t damage = 0;

	uint8_t strength = 0;
	uint8_t dexterity = 0;
	uint8_t endurance = 0;
	std::array<uint8_t, 3> bonusIdxes;
};

struct LevelBonus {
	uint8_t txtIdx = 0;
	bool bStatsModifier = false;
	bool bOffensive = false;
	bool bDefensive = false;
	void (*fnToInvoke)(uint8_t& , Entity*, Entity*) = nullptr;
};

// character types
enum class Character_Type : uint8_t {
	Outlaw = 0,
	Warrior,
	Barbarian,
	NOT_SET
};

struct Character: public  Entity{
	Character_Type classType = Character_Type::NOT_SET;
};

// monster types
enum class Monster_Type : uint8_t {
	Goblin = 0,
	Skeleton,
	Slime,
	Ghost,
	Golem,
	Dragon,
	NOT_SET
};

struct Monster : public  Entity {
	Monster_Type classType = Monster_Type::NOT_SET;
};