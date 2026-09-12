

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <spdlog/spdlog.h>

#include <array>

#include "screen.h"
#include "background.h"
#include "button_functions.h"
#include "localization.h"
#include "entity.h"
#include "animation.h"
#include "sprite.h"
#include "level_bonus_functions.h"

using Utility::vec2;
using Utility::GetRandomNumber;

namespace Graphics
{
	// macros below are for debug purposes only
	#define ALWAYS_ON_TOP

	#define XGA
	// 4:3
	#ifdef XGA
	#define W 1024
	#define H 768
	#endif

	#ifdef XGA_PLUS
	#define W 1152
	#define H 864
	#endif
	// 16:9
	#ifdef HDV_720
	#define W 1280 
	#define H 720
	#endif

	#ifdef HD_PLUS
	#define W 1600
	#define H 900
	#endif
	
	
	#undef XGA
	#undef XGA_PLUS
	#undef HDV_720
	#undef HD_PLUS


	constexpr uint16_t TargetWindowWidth = W;
	constexpr uint16_t TargetWindowHeight = H;

	uint16_t WindowWidth = TargetWindowWidth;
	uint16_t WindowHeight = TargetWindowHeight;

	SDL_Window* Window		= nullptr;
	SDL_Renderer* Renderer	= nullptr;

	//bool bWindowResized = false;
}

namespace System
{
	uint64_t millisecondsLastFrame = 0;
	uint64_t millisecondsDelta = 0;

	bool bMouseBtnDown = false;
	constexpr uint64_t buttonAABBThreshold = 50; // perform buttons aabb test only onece in 50 millisecons

	vec2<float> mouseCoords{};
}

namespace Anim
{
	enum class AnimType : uint8_t {
		Attack = 0,
		Idle,
		Hurt,
		Dead,
		NOT_SET
	};

	bool bIsActiveAnimationEnded = true;

	const std::array<AnimInfo, 3>* defPlayerAnimInfos = nullptr;
	const std::array<AnimInfo, 6>* defMonsterAnimInfos = nullptr;

	Animation* playerAnimations = nullptr;
	Animation* monsterAnimations = nullptr;

	Animation* attackerEntityAnimations = nullptr;
	Animation* victimEntityAnimations = nullptr;

	uint64_t strikeAnimFrameThreshold	= 200;
	uint64_t idleAnimFrameThreshold		= 200;
	uint64_t idleHurtAnimFrameThreshold = 200;
	uint64_t deathAnimFrameThreshold	= 200;

	uint64_t aestheticDelayThreshold = 1500;
	uint64_t aestheticDelay = 0;

	bool AestheticDelayCounter() {
		aestheticDelay += System::millisecondsDelta;

		if (aestheticDelay >= aestheticDelayThreshold) {
			aestheticDelay -= aestheticDelayThreshold;
			return true;
		}

		return false;
	}

	Animation* SetEntityAnimationByAnimType(Entity*, const AnimType&);
	void SetAnimationsByTypes();
	void AdvanceAnimations() {

		std::vector<Sprite>& sprites = (*UI::currentScreen).getSpriteEntries();
		for (Sprite& sprt : sprites) {
			if (sprt.bIsSpriteAnimated) { sprt.anim->AdvanceAnimation();}
		}	
	}
}

namespace Resources {
	
	TTF_Font* buttonsFont;
	TTF_Font* statsFont;
	TTF_Font* descLargeTextFont;
	TTF_Font* descSmallTextFont;
	TTF_Font* gameTitleFont;
	TTF_Font* descAboutTextFont;
	TTF_Font* statsFontSmall;


	std::array<TTF_Font*, 7>* fonts;

	std::array<TextureResourceEntry, 3>* ui_textures = nullptr;

	void LoadAnimTextures(Entity* entity) {

		if (entity->entType == EntityType::Character) {

			Character* character = static_cast<Character*>(entity);
			switch (character->classType) {
				case Character_Type::Outlaw: {
					Anim::playerAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Outlaw/Attack_1.png");
					Anim::playerAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Outlaw/Attack_2.png");
					Anim::playerAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Outlaw/Attack_3.png");
					Anim::playerAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Outlaw/Idle.png");
					Anim::playerAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Outlaw/Hurt.png");
					Anim::playerAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Outlaw/Dead.png");
				}break;
				case Character_Type::Warrior: {
					Anim::playerAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Warrior/Attack_1.png");
					Anim::playerAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Warrior/Attack_2.png");
					Anim::playerAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Warrior/Attack_3.png");
					Anim::playerAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Warrior/Idle.png");
					Anim::playerAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Warrior/Hurt.png");
					Anim::playerAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Warrior/Dead.png");
				}break;
				case Character_Type::Barbarian: {
					Anim::playerAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Barbarian/Attack_1.png");
					Anim::playerAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Barbarian/Attack_2.png");
					Anim::playerAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Barbarian/Attack_3.png");
					Anim::playerAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Barbarian/Idle.png");
					Anim::playerAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Barbarian/Hurt.png");
					Anim::playerAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/characters/Barbarian/Dead.png");
				}break;
				default:break;
			}
		}
		else if (entity->entType == EntityType::Monster) {

			using Anim::monsterAnimations;
			Monster* monster = static_cast<Monster*>(entity);
			switch (monster->classType) {
				case Monster_Type::Goblin: {
					Anim::monsterAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Goblin/Attack_1.png");
					Anim::monsterAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Goblin/Attack_2.png");
					Anim::monsterAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Goblin/Attack_3.png");
					Anim::monsterAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Goblin/Idle.png");
					Anim::monsterAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Goblin/Hurt.png");
					Anim::monsterAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Goblin/Dead.png");
				}break;
				case Monster_Type::Skeleton: {
					Anim::monsterAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Skeleton/Attack_1.png");
					Anim::monsterAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Skeleton/Attack_2.png");
					Anim::monsterAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Skeleton/Attack_3.png");
					Anim::monsterAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Skeleton/Idle.png");
					Anim::monsterAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Skeleton/Hurt.png");
					Anim::monsterAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Skeleton/Dead.png");
				}break;
				case Monster_Type::Slime: {
					Anim::monsterAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Slime/Attack_1.png");
					Anim::monsterAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Slime/Attack_2.png");
					Anim::monsterAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Slime/Attack_3.png");
					Anim::monsterAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Slime/Idle.png");
					Anim::monsterAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Slime/Hurt.png");
					Anim::monsterAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Slime/Dead.png");
				}break;
				case Monster_Type::Ghost: {
					Anim::monsterAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Ghost/Attack_1.png");
					Anim::monsterAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Ghost/Attack_2.png");
					Anim::monsterAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Ghost/Attack_3.png");
					Anim::monsterAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Ghost/Idle.png");
					Anim::monsterAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Ghost/Hurt.png");
					Anim::monsterAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Ghost/Dead.png");
				}break; 
				case Monster_Type::Golem: {
					Anim::monsterAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Golem/Attack_1.png");
					Anim::monsterAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Golem/Attack_2.png");
					Anim::monsterAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Golem/Attack_3.png");
					Anim::monsterAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Golem/Idle.png");
					Anim::monsterAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Golem/Hurt.png");
					Anim::monsterAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Golem/Dead.png");
				}break; 
				case Monster_Type::Dragon: {
					Anim::monsterAnimations->animTextures[0] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Dragon/Attack_1.png");
					Anim::monsterAnimations->animTextures[1] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Dragon/Attack_2.png");
					Anim::monsterAnimations->animTextures[2] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Dragon/Attack_3.png");
					Anim::monsterAnimations->animTextures[3] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Dragon/Idle.png");
					Anim::monsterAnimations->animTextures[4] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Dragon/Hurt.png");
					Anim::monsterAnimations->animTextures[5] = IMG_LoadTexture(Graphics::Renderer, "resources/images/monsters/Dragon/Dead.png");
				}break;
				default:break;
			}	
		}
	}

	void UnloadAnimResources(Entity* entity) {

		using Anim::playerAnimations;
		using Anim::monsterAnimations;

		if (entity->entType == EntityType::Character) {
			if (playerAnimations) {
				for (SDL_Texture* txt : playerAnimations->animTextures) SDL_DestroyTexture(txt);
			}
		}
		else if (entity->entType == EntityType::Monster) {
			if (monsterAnimations) {
				for (SDL_Texture* txt : monsterAnimations->animTextures) SDL_DestroyTexture(txt);
			}
		}
	}
}
//Resources

namespace Game
{
	bool bGameIsRunning = false;
	bool bNewRoundBegginig = true;

	// common gameplay variables
	const uint8_t victoriesToCompleteGame = 5;
	const uint8_t maxCharLevel = 3;
	const uint8_t monstersCount = 6;
	const uint8_t charMaxAtributeValue = 3;

	uint8_t victoryCount = 0;
	uint8_t currentRound = 1;
	uint8_t stepsMade = 0;
	uint8_t charsSelected = 0;
	Character_Type newlySelecterCharType = Character_Type::NOT_SET;
	
	// character pointers
	const std::array<Character, 3>* defaultCharacters = nullptr;

	Character* playerCharacter = nullptr;

	Entity* attacker	= nullptr;
	Entity* victim		= nullptr;

	std::array<Character_Type, 3>* selectedCharTypes = nullptr;

	// weapons pointers
	const std::array< Weapon, 6>* defaultWeapons = nullptr;

	Weapon* currentWeapon = nullptr;
	Weapon* newWeapon = nullptr;

	// monster pointers
	const std::array< Monster, 6>* defaultMonsters = nullptr;
	Monster* randomMonster = nullptr;

	//player stats for current round
	uint8_t charDefaultStats_strength  = 0;
	uint8_t charDefaultStats_dexterity = 0;
	uint8_t charDefaultStats_endurance = 0;

	uint8_t playerCurrRoundMaxHealth = 0;
	uint8_t mnstrCurrRoundMaxHealth = 0;

	uint8_t lastMonsterRandomIdx = 255;

	uint8_t spriteScale = 3;

	std::array<LevelBonus, 13>* levelBonuses = nullptr;

	void ResetGame() {

		victoryCount = 0;
		currentRound = 1;
		stepsMade = 0;
		charsSelected = 0;
		bNewRoundBegginig = true;

		(*selectedCharTypes)[0] = Character_Type::NOT_SET;
		(*selectedCharTypes)[1] = Character_Type::NOT_SET;
		(*selectedCharTypes)[2] = Character_Type::NOT_SET;

		Screen* CharacterSelectionScreen = (*UI::screens)[1];
		CharacterSelectionScreen->getButtons()[0].bIsToggledOn = false;
		CharacterSelectionScreen->getButtons()[0].bIsActive = true;

		CharacterSelectionScreen->getButtons()[1].bIsToggledOn = false;
		CharacterSelectionScreen->getButtons()[1].bIsActive = true;

		CharacterSelectionScreen->getButtons()[2].bIsToggledOn = false;
		CharacterSelectionScreen->getButtons()[2].bIsActive = true;

		CharacterSelectionScreen->getButtons()[3].bIsActive = false;
	}

	void ComputeStepOutcome() {

		++stepsMade;

		uint8_t hitChanse = GetRandomNumber(playerCharacter->dexterity + randomMonster->dexterity);
		if (victim->dexterity >= hitChanse) {
			// attack missed
			return;
		}

		uint8_t initialDamage = attacker->damage;

		//apply monster/hero damage bonuses
		if (attacker->entType == EntityType::Character)
		{
			for (const Character_Type& typ : *selectedCharTypes) {

				uint8_t charLevel = Game::playerCharacter->charLevel;
				if ((typ == Character_Type::NOT_SET) || (charLevel == 0)) break;

				uint8_t charIdx = static_cast<uint8_t>(typ);
				uint8_t lvlBonusIdx = (*Game::defaultCharacters)[charIdx].bonusIdxes[charLevel - 1];

				const LevelBonus& lvlBonus = (*Game::levelBonuses)[lvlBonusIdx];
				if (lvlBonus.bOffensive) {
					lvlBonus.fnToInvoke(initialDamage, Game::playerCharacter, Game::randomMonster);
				}
			}
		}
		else if (attacker->entType == EntityType::Monster) {

			Monster_Type typ = Game::randomMonster->classType;
			if ((typ != Monster_Type::Goblin) && (typ != Monster_Type::NOT_SET))
			{
				uint8_t mnstrIdx = static_cast<uint8_t>(typ);
				uint8_t lvlBonusIdx = (*Game::defaultMonsters)[mnstrIdx].bonusIdxes[0];
				const LevelBonus& lvlBonus = (*Game::levelBonuses)[lvlBonusIdx];
				if (lvlBonus.bOffensive) {
					lvlBonus.fnToInvoke(initialDamage, Game::randomMonster, Game::playerCharacter);
				}
			}
		}
		
		//apply monster/hero damage filtration
		if (victim->entType == EntityType::Character)
		{
			for (const Character_Type& typ : *selectedCharTypes) {

				uint8_t charLevel = Game::playerCharacter->charLevel;
				if ((typ == Character_Type::NOT_SET) || (charLevel == 0)) break;

				uint8_t charIdx = static_cast<uint8_t>(typ);
				uint8_t lvlBonusIdx = (*Game::defaultCharacters)[charIdx].bonusIdxes[charLevel - 1];

				const LevelBonus& lvlBonus = (*Game::levelBonuses)[lvlBonusIdx];
				if (lvlBonus.bDefensive) {
					lvlBonus.fnToInvoke(initialDamage, Game::randomMonster, Game::playerCharacter);
				}
			}
		}
		else if (victim->entType == EntityType::Monster) {

			Monster_Type typ = Game::randomMonster->classType;
			if ((typ != Monster_Type::Goblin) && (typ != Monster_Type::NOT_SET))
			{
				uint8_t mnstrIdx = static_cast<uint8_t>(typ);
				uint8_t lvlBonusIdx = (*Game::defaultMonsters)[mnstrIdx].bonusIdxes[0];
				const LevelBonus& lvlBonus = (*Game::levelBonuses)[lvlBonusIdx];
				if (lvlBonus.bDefensive) {
					lvlBonus.fnToInvoke(initialDamage, Game::playerCharacter, Game::randomMonster);
				}
			}
		}

		if (initialDamage > 0) {
			if (initialDamage >= victim->health) victim->health = 0;
			else victim->health -= initialDamage;
		}
	}

	void CheckRoundEndConditions() {
		// check victory or defeat conditions
		if (randomMonster->health == 0) {
			// victory, player won this round

			++victoryCount;
			if (victoryCount >= victoriesToCompleteGame) {
				// player completed the game

				UI::currentScreen = (*UI::screens)[6];
				return;
			}

			++currentRound;
			stepsMade = 0;
			bNewRoundBegginig = true;

			UI::currentScreen = (*UI::screens)[3];
			return;
		}
		else if(playerCharacter->health == 0) {
			// player defeated
			UI::currentScreen = (*UI::screens)[5];
			return;
		}
	}

	void UpdateSpritePositions() {
		//update player sprite positions
		uint8_t charClassType = static_cast<uint8_t>(playerCharacter->classType);
		float charHeight = (*Anim::defPlayerAnimInfos)[charClassType].images_h;

		float floor = Graphics::WindowHeight / 2 + Graphics::WindowHeight / 16;
		float charHorizontalPos = Graphics::WindowWidth / 2 - charHeight * Game::spriteScale/2.8f;
		float charVerticalPos = floor - charHeight * Game::spriteScale;

		std::vector<Sprite>& sprite = (*UI::screens)[2]->getSpriteEntries();
		sprite[0].pos.x = charHorizontalPos;
		sprite[0].pos.y = charVerticalPos;

		//update monster sprite positions
		uint8_t monsterClassType = static_cast<uint8_t>(randomMonster->classType);
		float monsterHeight = (*Anim::defMonsterAnimInfos)[monsterClassType].images_h;

		float monsterHorizontalPos = Graphics::WindowWidth / 2 - monsterHeight * Game::spriteScale/1.5f;
		float monsterVerticalPos = floor - monsterHeight * Game::spriteScale;

		sprite[1].pos.x = monsterHorizontalPos;
		sprite[1].pos.y = monsterVerticalPos;

	}

	void SwapAttackerAndVictim() {
		if (stepsMade > 0) {
			if (attacker == playerCharacter) {
				attacker = randomMonster;
				victim = playerCharacter;
			}
			else {
				attacker = playerCharacter;
				victim = randomMonster;
			}
		}
	}

	void InitPlayer() {
		// this code should be executed every new game run

		Character_Type chType = (*selectedCharTypes)[0];
		uint8_t charIdx = static_cast<uint8_t>(chType);

		// set selected char name
		std::vector<TextEntry>& BattleOn_TextEntries = (*UI::screens)[2]->getTextEntries();
		BattleOn_TextEntries[3].textIdx = charIdx + 30;

		// copy-init player character with default values
		*playerCharacter = (*defaultCharacters)[charIdx];

		// set correspondig to this character weapon
		uint8_t wpnIdx = (*playerCharacter).wpnIdx;
		*currentWeapon = (*defaultWeapons)[wpnIdx];

		charDefaultStats_strength = GetRandomNumber(charMaxAtributeValue);
		charDefaultStats_dexterity = GetRandomNumber(charMaxAtributeValue);
		charDefaultStats_endurance = GetRandomNumber(charMaxAtributeValue);

		Resources::UnloadAnimResources(playerCharacter);
		Resources::LoadAnimTextures(playerCharacter);	
	}

	void ResetPlayer() {

		// restore player characters randomized health and stats

		playerCharacter->strength = charDefaultStats_strength;
		playerCharacter->dexterity = charDefaultStats_dexterity;
		playerCharacter->endurance = charDefaultStats_endurance;

		// here do player stat upgrade according to level bonuses, if any available

		for (const Character_Type& typ : *selectedCharTypes) {

			uint8_t charLevel = Game::playerCharacter->charLevel;
			if((typ == Character_Type::NOT_SET) || (charLevel == 0)) break;

			uint8_t charIdx = static_cast<uint8_t>(typ);
			uint8_t lvlBonusIdx = (*Game::defaultCharacters)[charIdx].bonusIdxes[charLevel - 1];

			LevelBonus* lvlBonus = &(*Game::levelBonuses)[lvlBonusIdx];
			if (lvlBonus->bStatsModifier) {
				uint8_t temp = 0;
				lvlBonus->fnToInvoke(temp, Game::playerCharacter, nullptr);
			}
		}

		/*
		for (Character_Type typ : *selectedCharTypes) {
			if (typ != Character_Type::NOT_SET) {
				switch (typ)
				{
				case Character_Type::Outlaw: {
					if (playerCharacter->charLevel == 2) playerCharacter->dexterity += 1;
				}break;

				case Character_Type::Warrior: {
					if (playerCharacter->charLevel == 3) playerCharacter->strength += 1;
				}break;

				case Character_Type::Barbarian: {
					if (playerCharacter->charLevel == 3) playerCharacter->endurance += 1;
				}break;
				default: break;
				}

			}
		}
		*/

		Character_Type chType = (*selectedCharTypes)[0];
		uint8_t charIdx = static_cast<uint8_t>(chType);

		uint8_t playerDefHealth = (*defaultCharacters)[charIdx].health;
		playerCharacter->health = playerDefHealth + playerDefHealth * playerCharacter->charLevel;
		playerCharacter->health += playerCharacter->endurance * playerCharacter->charLevel;
		playerCurrRoundMaxHealth = playerCharacter->health;

		uint8_t wndIdx = playerCharacter->wpnIdx;
		playerCharacter->damage = (*defaultWeapons)[wndIdx].damage + playerCharacter->strength;
		//playerCharacter->damage = playerCharacter->strength;
			
	}

	void InitRandMonster() {

		// get new random monster and its corresponding weapon drop
		uint8_t rndMnstrIdx = static_cast<uint8_t>(SDL_rand(Game::monstersCount));

		while (true) {
			if (rndMnstrIdx != lastMonsterRandomIdx) { break; }
			rndMnstrIdx = static_cast<uint8_t>(SDL_rand(Game::monstersCount));
		}

		lastMonsterRandomIdx = rndMnstrIdx;

		// copy-init monster with default values
		*randomMonster = (*defaultMonsters)[rndMnstrIdx];
		mnstrCurrRoundMaxHealth = randomMonster->health;

		//set appropriate monster name for ui
		std::vector<TextEntry>& BattleOn_TextEntries = (*UI::screens)[2]->getTextEntries();
		BattleOn_TextEntries[4].textIdx = rndMnstrIdx + 33;

		Resources::UnloadAnimResources(randomMonster);
		Resources::LoadAnimTextures(randomMonster);

		uint8_t wndIdx = (*randomMonster).wpnIdx;
		*newWeapon = (*defaultWeapons)[wndIdx];
	}

	void DecideWhoStrikesFirst() {

		// before first fight of each round, decide who strikes first
		if (playerCharacter->dexterity >= randomMonster->dexterity) {
			attacker = playerCharacter;
			victim = randomMonster;
		}
		else {
			attacker = randomMonster;
			victim = playerCharacter;
		}
	}
}
//Game

namespace UI
{
	Screen* currentScreen;
	std::array<Screen*, 9>* screens;
	Background* rndBackground;

	ButtonType* buttonTypes;

	SDL_Color btnDefColor = { 255, 255, 255, 255 };
	SDL_Color btnDescColor = { 255, 255, 255, 0 };
	SDL_Color btnDefBg = { 100, 100, 100, 255 };

	SDL_Color statsColoredText = { 80, 255, 50, 0 };

	float buttonsFontSize = 50.f;
	float statsFontSize = 25.f;
	float descLargeTextFontSize = 80.f;
	float descSmallTextFontSize = 40.f;
	float descAboutTextFontSize = 30.f;
	float gameTitleFontSize = 200.f;
	float statsFontSmallSize = 15.f;


	std::array<const std::string, 30>* btnText = nullptr;
	std::array<const std::string, 60>* descText = nullptr;

	uint8_t statsFont = 1;

	uint8_t overlapedBtnIndex = 0; // if aabb is positive, this variable is set to button index within the screen`s vector of buttons
	bool bOverlapedBtnIndexFound = false;
	bool bOverlapedBtnAletered = true;

	constexpr uint64_t uiBtnHoldThreshold = 50;
	bool bUiBtnHoldThresholdMet = false;
	uint64_t uiBtnHoldDuration = 0;
	uint64_t aabbDuration = 0;

	float defHealthBarHeight = 500.0f;
	SDL_Color hlthBarClr = { 255, 0, 0, 255 };
	SDL_FRect playerHealthBar = { 0.0f, 0.0f, 50.0f, defHealthBarHeight };
	SDL_FRect mnstrHealthBar = { 0.0f, 0.0f, 50.0f, defHealthBarHeight };

	void AABBMouseVsButtons() {

		aabbDuration += System::millisecondsDelta; // each frame increment aabb threshold counter by delta

		if (aabbDuration >= System::buttonAABBThreshold) {

			aabbDuration = 0;

			float mouseX = System::mouseCoords.x;
			float mouseY = System::mouseCoords.y;

			/*
			SDL_FRect frect;
			SDL_GetRenderLogicalPresentationRect(Graphics::Renderer, &frect);

			float widthFactor = static_cast<float>(frect.w) / static_cast<float>(Graphics::TargetWindowWidth);
			float heightFactor = static_cast<float>(frect.h) / static_cast<float>(Graphics::TargetWindowHeight);

			SDL_Log("w: %f, h: %f, wF: %f, hF: %f", frect.w, frect.h, widthFactor, heightFactor);
			*/

			/*
			int width = 0, height = 0;
			SDL_GetRenderOutputSize(Graphics::Renderer, &width, &height);

			float widthFactor =	static_cast<float>(width) / static_cast<float>(Graphics::TargetWindowWidth);
			float heightFactor = static_cast<float>(height) / static_cast<float>(Graphics::TargetWindowHeight);
			SDL_Log("w: %i, h: %i, wF: %f, hF: %f", width, height, widthFactor, heightFactor);
			*/




			//float widthFactor =	static_cast<float>(Graphics::WindowWidth) / static_cast<float>(Graphics::TargetWindowWidth);
			//float heightFactor = static_cast<float>(Graphics::WindowHeight) / static_cast<float>(Graphics::TargetWindowHeight);
			//SDL_Log("w: %d, h: %d, wF: %f, hF: %f", Graphics::WindowWidth, Graphics::WindowHeight, widthFactor, heightFactor);



			for (size_t idx = 0; idx < UI::currentScreen->buttonCount; ++idx) {

				uint8_t btnTypeIdx = UI::currentScreen->getButtons()[idx].btnTypeIdx;
				uint16_t w = buttonTypes->getBtnDimsByTypeIdx(btnTypeIdx).x;
				uint16_t h = buttonTypes->getBtnDimsByTypeIdx(btnTypeIdx).y;

				float buttonLeftX = UI::currentScreen->getButtons()[idx].pos.x;
				float buttonRightX = UI::currentScreen->getButtons()[idx].pos.x + w;
				float buttonTopY = UI::currentScreen->getButtons()[idx].pos.y;
				float buttonBottomY = UI::currentScreen->getButtons()[idx].pos.y + h;

				// AABB interseciton check performed here
				if ((mouseX >= buttonLeftX) && (mouseX <= buttonRightX) && (mouseY >= buttonTopY) && (mouseY <= buttonBottomY)) {
					UI::overlapedBtnIndex = idx;
					UI::bOverlapedBtnIndexFound = true;
					break;
				}
				else {
					UI::bOverlapedBtnIndexFound = false;
				}
			}
		}
	}

	void ProcessButtons() {
		if (!System::bMouseBtnDown) UI::bOverlapedBtnAletered = false;

		if (UI::bOverlapedBtnIndexFound && System::bMouseBtnDown) {

			if (!UI::bOverlapedBtnAletered) {

				UI::uiBtnHoldDuration += System::millisecondsDelta;

				if (UI::uiBtnHoldDuration >= UI::uiBtnHoldThreshold) {
					UI::bUiBtnHoldThresholdMet = true;
				}
			}
		}
		else if (!UI::bOverlapedBtnIndexFound && System::bMouseBtnDown) {
			UI::bOverlapedBtnAletered = true;
			UI::uiBtnHoldDuration = 0;
			UI::bUiBtnHoldThresholdMet = false;
		}

		if (!System::bMouseBtnDown && UI::bUiBtnHoldThresholdMet && !UI::bOverlapedBtnAletered) {
			std::vector<Button>& screenButtons = currentScreen->getButtons();
			Button& overlapedBtn = screenButtons[UI::overlapedBtnIndex];

			if (overlapedBtn.bIsActive) {
				if (overlapedBtn.bIsTogglable) { // handlign of togglable buttons

					if (!overlapedBtn.bIsToggledOn) {
						//overlapedBtn.toggleBtn();
						overlapedBtn.fnToInvoke();
					}
					//else overlapedBtn.toggleBtn();
				}
				else overlapedBtn.fnToInvoke();
			}

			UI::uiBtnHoldDuration = 0;
			UI::bUiBtnHoldThresholdMet = false;
		}
	}

	void DrawWeaponIconAtLocation(uint8_t wpnIdx, const Utility::vec2<uint16_t>& pos,
		const TextureResourceEntry& wpn_icons, const Weapon& defWpnStats) {
		
		// draw wpn icon
		SDL_FRect src{};
		SDL_FRect dest{};
		SDL_Texture* txt = wpn_icons.txt;

		src.x = wpn_icons.dims.y * wpnIdx;
		src.y = 0;
		src.w = wpn_icons.dims.y;
		src.h = wpn_icons.dims.y;

		dest.x = pos.x;
		dest.y = pos.y;
		dest.w = wpn_icons.dims.y;
		dest.h = wpn_icons.dims.y;

		SDL_RenderTexture(Graphics::Renderer, txt, &src, &dest);

		// draw weapon name
		const std::string& wpnName = (*UI::descText)[defWpnStats.txtIdx];
		TTF_Font* font = (*Resources::fonts)[UI::statsFont];

		int textWidth = 0;
		int textHeight = 0;

		TTF_GetStringSize(font, wpnName.c_str(), 0, &textWidth, &textHeight);

		SDL_Surface* txtSurface = TTF_RenderText_Shaded(font, wpnName.c_str(), 0, UI::statsColoredText, SDL_Color{});

		dest.x = pos.x;
		dest.y = pos.y - textHeight;
		dest.w = textWidth;
		dest.h = textHeight;

		txt = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
		SDL_DestroySurface(txtSurface);

		SDL_RenderTexture(Graphics::Renderer, txt, NULL, &dest);
		SDL_DestroyTexture(txt);

		// draw weapon damage text
		const std::string& wpnDmg = (*UI::descText)[19];
		int wpnDmgTextWidth = 0;
		TTF_GetStringSize(font, wpnDmg.c_str(), 0, &wpnDmgTextWidth, &textHeight);

		txtSurface = TTF_RenderText_Shaded(font, wpnDmg.c_str(), 0, UI::btnDescColor, SDL_Color{});

		dest.x = pos.x;
		dest.y = pos.y + wpn_icons.dims.y;
		dest.w = wpnDmgTextWidth;
		dest.h = textHeight;

		txt = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
		SDL_DestroySurface(txtSurface);

		SDL_RenderTexture(Graphics::Renderer, txt, NULL, &dest);
		SDL_DestroyTexture(txt);

		// draw weapon damage value
		uint8_t val = defWpnStats.damage;
		std::string wpnDmgVal =  std::to_string(val);
		TTF_GetStringSize(font, wpnDmgVal.c_str(), 0, &textWidth, &textHeight);

		txtSurface = TTF_RenderText_Shaded(font, wpnDmgVal.c_str(), 0, UI::statsColoredText, SDL_Color{});

		dest.x = pos.x + wpnDmgTextWidth + 10;
		dest.y = pos.y + wpn_icons.dims.y;
		dest.w = textWidth;
		dest.h = textHeight;

		txt = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
		SDL_DestroySurface(txtSurface);

		SDL_RenderTexture(Graphics::Renderer, txt, NULL, &dest);
		SDL_DestroyTexture(txt);

		// draw weapon damage type text
		const std::string& wpnDmgType = (*UI::descText)[20];
		TTF_GetStringSize(font, wpnDmgType.c_str(), 0, &textWidth, &textHeight);

		txtSurface = TTF_RenderText_Shaded(font, wpnDmgType.c_str(), 0, UI::btnDescColor, SDL_Color{});

		dest.x = pos.x;
		dest.y = pos.y + wpn_icons.dims.y + textHeight;
		dest.w = textWidth;
		dest.h = textHeight;

		txt = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
		SDL_DestroySurface(txtSurface);

		SDL_RenderTexture(Graphics::Renderer, txt, NULL, &dest);
		SDL_DestroyTexture(txt);

		// draw weapon damage type value
		uint8_t wpnDmgTypeIdx = static_cast<uint8_t>(defWpnStats.dmgType);
		int wpnDmgTypeValWidth = 0;
		const std::string& wpnDmgTypeValue = (*UI::descText)[wpnDmgTypeIdx + 21];
		TTF_GetStringSize(font, wpnDmgTypeValue.c_str(), 0, &wpnDmgTypeValWidth, &textHeight);

		txtSurface = TTF_RenderText_Shaded(font, wpnDmgTypeValue.c_str(), 0, UI::statsColoredText, SDL_Color{});

		dest.x = pos.x + textWidth + 10;
		dest.y = pos.y + wpn_icons.dims.y + textHeight;
		dest.w = wpnDmgTypeValWidth;
		dest.h = textHeight;

		txt = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
		SDL_DestroySurface(txtSurface);

		SDL_RenderTexture(Graphics::Renderer, txt, NULL, &dest);
		SDL_DestroyTexture(txt);		
	}

	void DrawStatsAtLocation(Entity* entity, vec2<uint16_t> pos) {

		//stats text
		std::string str = (*descText)[24];

		int textWidth = 0;
		int textHeight = 0;
		int textValWidth = 0;
		int textValHeight = 0;

		TTF_GetStringSizeWrapped(Resources::statsFont, str.c_str(), 0,0, &textWidth, &textHeight);
		
		SDL_FRect dest{};
		dest.x = pos.x;
		dest.y = pos.y;
		dest.w = textWidth;
		dest.h = textHeight;

		SDL_Surface* txtSurface = nullptr;
		txtSurface = TTF_RenderText_Shaded_Wrapped(Resources::statsFont, str.c_str(), 0, UI::btnDescColor, SDL_Color{}, 0);
		SDL_Texture* texture = nullptr;
		texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
		SDL_DestroySurface(txtSurface);
	
		SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
		SDL_DestroyTexture(texture);

		textWidth += textWidth * 0.1f;
		uint16_t statVerticalPos = 0;
		// strength val
		{
			uint8_t val = entity->strength;
			str = std::to_string(val);

			TTF_GetStringSize(Resources::statsFont, str.c_str(), 0, &textValWidth, &textValHeight);
			dest.x = pos.x + textWidth;
			dest.y = pos.y + statVerticalPos;
			dest.w = textValWidth;
			dest.h = textValHeight;
			statVerticalPos += textValHeight;

			txtSurface = TTF_RenderText_Shaded(Resources::statsFont, str.c_str(), 0, UI::statsColoredText, SDL_Color{});
			texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
			SDL_DestroySurface(txtSurface);

			SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
			SDL_DestroyTexture(texture);
		}

		// dexterity val
		{
			uint8_t val = entity->dexterity;
			str = std::to_string(val);

			TTF_GetStringSize(Resources::statsFont, str.c_str(), 0, &textValWidth, &textValHeight);
			dest.x = pos.x + textWidth;
			dest.y = pos.y + statVerticalPos;
			dest.w = textValWidth;
			dest.h = textValHeight;
			statVerticalPos += textValHeight;

			txtSurface = TTF_RenderText_Shaded(Resources::statsFont, str.c_str(), 0, UI::statsColoredText, SDL_Color{});
			texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
			SDL_DestroySurface(txtSurface);

			SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
			SDL_DestroyTexture(texture);
		}

		// endurance val
		{
			uint8_t val = entity->endurance;
			str = std::to_string(val);

			TTF_GetStringSize(Resources::statsFont, str.c_str(), 0, &textValWidth, &textValHeight);
			dest.x = pos.x + textWidth;
			dest.y = pos.y + statVerticalPos;
			dest.w = textValWidth;
			dest.h = textValHeight;
			statVerticalPos += textValHeight;

			txtSurface = TTF_RenderText_Shaded(Resources::statsFont, str.c_str(), 0, UI::statsColoredText, SDL_Color{});
			texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
			SDL_DestroySurface(txtSurface);

			SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
			SDL_DestroyTexture(texture);
		}

		// damage val
		{
			uint8_t val = entity->damage;
			str = std::to_string(val);
			TTF_GetStringSize(Resources::statsFont, str.c_str(), 0, &textValWidth, &textValHeight);

			dest.x = pos.x + textWidth;
			dest.y = pos.y + statVerticalPos;
			dest.w = textValWidth;
			dest.h = textValHeight;
			statVerticalPos += textValHeight;

			txtSurface = TTF_RenderText_Shaded(Resources::statsFont, str.c_str(), 0, UI::statsColoredText, SDL_Color{});
			texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
			SDL_DestroySurface(txtSurface);

			SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
			SDL_DestroyTexture(texture);
		}

		//level bonuses text
		{
			if(entity->entType == EntityType::Character) str = (*descText)[25];
			else if(entity->entType == EntityType::Monster) str = (*descText)[27];

			TTF_GetStringSizeWrapped(Resources::statsFont, str.c_str(), 0, 0, &textValWidth, &textValHeight);
			statVerticalPos += 20;
			//SDL_FRect dest{};
			dest.x = pos.x;
			dest.y = pos.y + statVerticalPos;
			dest.w = textValWidth;
			dest.h = textValHeight;
			statVerticalPos += textValHeight;

			txtSurface = TTF_RenderText_Shaded_Wrapped(Resources::statsFont, str.c_str(), 0, UI::btnDescColor, SDL_Color{}, 0);
			texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
			SDL_DestroySurface(txtSurface);

			SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
			SDL_DestroyTexture(texture);
		}
		
		// level bonus values
		
		if (entity->entType == EntityType::Character)
		{
			for (const Character_Type& chTp : *Game::selectedCharTypes) {

				uint8_t charLevel = entity->charLevel;
				if( (chTp == Character_Type::NOT_SET) || (charLevel == 0)) break;
				
				uint8_t charIdx = static_cast<uint8_t>(chTp);
				uint8_t lvlBonusIdx = (*Game::defaultCharacters)[charIdx].bonusIdxes[charLevel - 1];

				LevelBonus* lvlBonus = &(*Game::levelBonuses)[lvlBonusIdx];
				str = (*descText)[lvlBonus->txtIdx];

				TTF_GetStringSizeWrapped(Resources::statsFontSmall, str.c_str(), 0, 0, &textValWidth, &textValHeight);
				dest.x = pos.x - 15;
				dest.y = pos.y + statVerticalPos;
				dest.w = textValWidth;
				dest.h = textValHeight;
				statVerticalPos += textValHeight + 10;

				txtSurface = TTF_RenderText_Shaded_Wrapped(Resources::statsFontSmall, str.c_str(), 0, UI::statsColoredText, SDL_Color{}, 0);
				texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
				SDL_DestroySurface(txtSurface);

				SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
				SDL_DestroyTexture(texture);
			}
		}
		else if (entity->entType == EntityType::Monster)
		{
			uint8_t lvlBonusIdx = entity->bonusIdxes[0];
			LevelBonus* lvlBonus = &(*Game::levelBonuses)[lvlBonusIdx];
			
			if (lvlBonusIdx != 0) str = (*descText)[lvlBonus->txtIdx];
			else str = "";

			TTF_GetStringSizeWrapped(Resources::statsFontSmall, str.c_str(), 0, 0, &textValWidth, &textValHeight);
			dest.x = pos.x - 15;
			dest.y = pos.y + statVerticalPos;
			dest.w = textValWidth;
			dest.h = textValHeight;
			statVerticalPos += textValHeight;
			UI::statsFontSmallSize;
			txtSurface = TTF_RenderText_Shaded_Wrapped(Resources::statsFontSmall, str.c_str(), 0, UI::statsColoredText, SDL_Color{},0);
			texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
			SDL_DestroySurface(txtSurface);

			SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
			SDL_DestroyTexture(texture);
		}

		// character level val
		if (entity->entType == EntityType::Character) {

			const TextEntry& levelTxt = (UI::currentScreen->getTextEntries())[5];
			

			uint8_t val = entity->charLevel;
			str = std::to_string(val);
			int width = 0;
			int height = 0;
			TTF_GetStringSize(Resources::statsFont, str.c_str(), 0, &width, &height);
			dest.x = levelTxt.pos.x + 80;
			dest.y = levelTxt.pos.y;
			dest.w = width;
			dest.h = height;

			txtSurface = TTF_RenderText_Shaded(Resources::statsFont, str.c_str(), 0, UI::statsColoredText, SDL_Color{});
			texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
			SDL_DestroySurface(txtSurface);

			SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
			SDL_DestroyTexture(texture);
		
			// round number val
			{
				uint8_t val = Game::currentRound;
				str = std::to_string(val);

				TTF_GetStringSize(Resources::descSmallTextFont, str.c_str(), 0, &textValWidth, &textValHeight);

				const TextEntry& levelTxt = (UI::currentScreen->getTextEntries())[6];

				dest.x = levelTxt.pos.x + 120;
				dest.y = levelTxt.pos.y;
				dest.w = textValWidth;
				dest.h = textValHeight;


				txtSurface = TTF_RenderText_Shaded(Resources::descSmallTextFont, str.c_str(), 0, UI::btnDescColor, SDL_Color{});
				texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
				SDL_DestroySurface(txtSurface);

				SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
				SDL_DestroyTexture(texture);
			}
		}
		
	}

	void DrawStats() {

		// draw battle_on ui stats
		if (currentScreen->scrnType == ScreenType::BattleOn) {

			const std::vector<TextEntry>& textEntries = (*currentScreen).getTextEntries();
			DrawStatsAtLocation(Game::playerCharacter, textEntries[1].pos);
			DrawStatsAtLocation(Game::randomMonster, textEntries[2].pos);
		}
	}

	void DrawEntityHealth(Entity* entity, vec2<uint16_t> pos) {

		// entity health val
		uint8_t val = entity->health;
		std::string str = std::to_string(val);

		int textValWidth = 0;
		int textValHeight = 0;

		TTF_GetStringSize(Resources::descSmallTextFont, str.c_str(), 0, &textValWidth, &textValHeight);
		SDL_FRect dest{};

		SDL_SetRenderDrawColor(Graphics::Renderer, hlthBarClr.r, hlthBarClr.g, hlthBarClr.b, hlthBarClr.a);
		if (entity->entType == EntityType::Character) {
			TextEntry& levelTxt = (UI::currentScreen->getTextEntries())[7];
			dest.x = levelTxt.pos.x + 25;
			dest.y = levelTxt.pos.y - 50;

			SDL_RenderFillRect(Graphics::Renderer, &UI::playerHealthBar);
		}
		else if (entity->entType == EntityType::Monster) {
			TextEntry& levelTxt = (UI::currentScreen->getTextEntries())[8];
			dest.x = levelTxt.pos.x + 25;
			dest.y = levelTxt.pos.y - 50;

			SDL_RenderFillRect(Graphics::Renderer, &UI::mnstrHealthBar);
		}

		dest.w = textValWidth;
		dest.h = textValHeight;

		SDL_Surface* txtSurface = TTF_RenderText_Shaded(Resources::statsFont, str.c_str(), 0, UI::btnDescColor, SDL_Color{});
		SDL_Texture* texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
		SDL_DestroySurface(txtSurface);

		SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
		SDL_DestroyTexture(texture);
	}

	void DrawHealthBarsAndHealth() {

		if (currentScreen->scrnType == ScreenType::BattleOn) {
			const std::vector<TextEntry>& textEntries = (*currentScreen).getTextEntries();
			DrawEntityHealth(Game::playerCharacter, textEntries[1].pos);
			DrawEntityHealth(Game::randomMonster, textEntries[2].pos);
		}
	}

	void DrawWeaponIcon() {

		// draw battle on ui, weapon stats and icon and other stats
		if (currentScreen->scrnType == ScreenType::BattleOn) {

			// draw player weapon icon with stats
			const std::vector<TextEntry>& textEntries = (*currentScreen).getTextEntries();
			const vec2<uint16_t> wpnIconPos = textEntries[0].pos;
			const TextureResourceEntry& wpnIcons = (*Resources::ui_textures)[0];
			uint8_t wpnIdx = static_cast<uint8_t>((*Game::currentWeapon).wpnType);
			DrawWeaponIconAtLocation(wpnIdx, wpnIconPos, wpnIcons, (*Game::defaultWeapons)[wpnIdx]);
		}
		else if (currentScreen->scrnType == ScreenType::VictoryUpgradeWeapon) {
			// draw player weapon icon
			const std::vector<TextEntry>& textEntries = (*currentScreen).getTextEntries();
			const vec2<uint16_t> playerWpnIconPos = textEntries[2].pos;
			const TextureResourceEntry& wpnIcons = (*Resources::ui_textures)[0];
			uint8_t playerWpnIdx = static_cast<uint8_t>((*Game::currentWeapon).wpnType);
			DrawWeaponIconAtLocation(playerWpnIdx, playerWpnIconPos, wpnIcons, (*Game::defaultWeapons)[playerWpnIdx]);

			// draw new weapon icon
			const vec2<uint16_t> newWpnIconPos = textEntries[3].pos;
			uint8_t newWpnIdx = static_cast<uint8_t>((*Game::newWeapon).wpnType);
			DrawWeaponIconAtLocation(newWpnIdx, newWpnIconPos, wpnIcons, (*Game::defaultWeapons)[newWpnIdx]);
		}

	}

	void DrawTextDescriptions(){

		// in tis function we render current screen`s background,
		// and then we draw all ui text descriptions

		// draw current screen background image
		if (currentScreen->backgroundImage) SDL_RenderTexture(Graphics::Renderer, currentScreen->backgroundImage, 0, 0);

		// draw ui text descriptions
		const std::vector<TextEntry>& screenTextEntries = currentScreen->getTextEntries();
		const uint8_t txtEntriesCount = currentScreen->textEntriesCount;
		int textWidth = 0;
		int textHeight = 0;
		SDL_FRect dest{};

		for (int8_t idx = 0; idx < txtEntriesCount; ++idx) {
			if (screenTextEntries[idx].bIsVisible) {
				uint8_t txtIdx = screenTextEntries[idx].textIdx;
				SDL_Surface* txtSurface;
				SDL_Texture* texture = nullptr;

				const char* str = (*descText)[txtIdx].c_str();
				uint8_t fontIdx = screenTextEntries[idx].fontIdx;
				TTF_Font* font = (*Resources::fonts)[fontIdx];

				TTF_GetStringSizeWrapped(font, str, 0, 0, &textWidth, &textHeight);

				txtSurface = TTF_RenderText_Shaded_Wrapped(font, str, 0, UI::btnDescColor, SDL_Color{}, 0);

				dest.x = screenTextEntries[idx].pos.x;
				dest.y = screenTextEntries[idx].pos.y;
				dest.w = textWidth;
				dest.h = textHeight;

				texture = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
				SDL_DestroySurface(txtSurface);

				SDL_RenderTexture(Graphics::Renderer, texture, NULL, &dest);
				SDL_DestroyTexture(texture);
			}
		}
	}

	void DrawButtons() {

		//draw all buttons of current screen in their respective states and positions
		SDL_FRect src{};
		SDL_FRect dest{};
		uint8_t btnTypeIdx = 0;
		uint16_t btnWidth = 0;
		uint16_t btnHeight = 0;
		SDL_Texture* txt = nullptr;

		const std::vector<Button>& buttons = currentScreen->getButtons();
		const uint8_t btnCount = currentScreen->buttonCount;

		for (int8_t idx = 0; idx < btnCount; ++idx) {

			btnTypeIdx = buttons[idx].btnTypeIdx;
			btnWidth = buttonTypes->getBtnDimsByTypeIdx(btnTypeIdx).x;
			btnHeight = buttonTypes->getBtnDimsByTypeIdx(btnTypeIdx).y;

			src.x = 0;
			src.y = 0;
			src.w = btnWidth;
			src.h = btnHeight;

			dest.x = buttons[idx].pos.x;
			dest.y = buttons[idx].pos.y;
			dest.w = btnWidth;
			dest.h = btnHeight;

			txt = buttonTypes->getBtnTextureByTypeIdx(btnTypeIdx);
			if (buttons[idx].bIsActive) {

				if (buttons[idx].bIsTogglable && buttons[idx].bIsToggledOn) {
					src.y = btnHeight * 4;
					SDL_RenderTexture(Graphics::Renderer, txt, &src, &dest);
				}
				else if (System::bMouseBtnDown && bOverlapedBtnIndexFound && idx == overlapedBtnIndex) { // draw green
					src.y = btnHeight * 2;
					SDL_RenderTexture(Graphics::Renderer, txt, &src, &dest);
				}
				else if (bOverlapedBtnIndexFound && idx == overlapedBtnIndex) { // draw blue
					src.y = btnHeight;
					SDL_RenderTexture(Graphics::Renderer, txt, &src, &dest);
				}
				else {
					SDL_RenderTexture(Graphics::Renderer, txt, &src, &dest); // draw gray
				}
			}
			else {
				src.y = btnHeight * 3;
				SDL_RenderTexture(Graphics::Renderer, txt, &src, &dest);
			}
		}

		//draw button text
		SDL_Surface* txtSurface = nullptr;
		uint8_t txtIdx = 0;
		size_t charCount = 0;
		int textWidth = 0;
		int textHeight = 0;
		const char* str = nullptr;

		for (int8_t idx = 0; idx < btnCount; ++idx) {

			txtIdx = buttons[idx].btnTextId;

			charCount = (*btnText)[txtIdx].size();
			str = (*btnText)[txtIdx].c_str();

			TTF_GetStringSizeWrapped(Resources::buttonsFont, str, 0, 0, &textWidth, &textHeight);

			txtSurface = TTF_RenderText_Shaded(Resources::buttonsFont, str, charCount,
				UI::btnDefColor, SDL_Color{});

			btnTypeIdx = buttons[idx].btnTypeIdx;
			btnWidth = UI::buttonTypes->getBtnDimsByTypeIdx(btnTypeIdx).x;
			btnHeight = UI::buttonTypes->getBtnDimsByTypeIdx(btnTypeIdx).y;

			dest.x = buttons[idx].pos.x + (btnWidth - textWidth) / 2;
			dest.y = buttons[idx].pos.y + (btnHeight - textHeight) / 2;
			dest.w = textWidth;
			dest.h = textHeight;

			txt = SDL_CreateTextureFromSurface(Graphics::Renderer, txtSurface);
			SDL_DestroySurface(txtSurface);

			SDL_RenderTexture(Graphics::Renderer, txt, NULL, &dest);
			SDL_DestroyTexture(txt);
		}

		
	}

	void DrawAnimatedSprites() {

		if (Game::charsSelected != 0) {

			std::vector<Sprite>& sprites = currentScreen->getSpriteEntries();
			SDL_FRect src{};
			SDL_FRect dest{};
			uint16_t spriteWidth = 0;
			uint16_t spriteHeight = 0;

			for (Sprite& sprt : sprites) {
				if (sprt.bIsSpriteAnimated) {

					uint8_t textIdx = sprt.anim->currentAnimTxtIdx;

					Entity* entity = nullptr;
					entity = sprt.anim->entity;

					const AnimInfo* animInfo = nullptr;
					if (entity->entType == EntityType::Character) {
						uint8_t classType = static_cast<uint8_t>(Game::playerCharacter->classType);
						animInfo = &(*Anim::defPlayerAnimInfos)[classType];
					}
					else if (entity->entType == EntityType::Monster) {
						uint8_t classType = static_cast<uint8_t>(Game::randomMonster->classType);
						animInfo = &(*Anim::defMonsterAnimInfos)[classType];
					}
					//spriteWidth = animInfo->imageWidths[textIdx] / animInfo->frameCounts[textIdx];
					spriteHeight = animInfo->images_h;

					src.x = spriteHeight * sprt.anim->currentFrame;
					src.y = 0;
					src.w = spriteHeight;
					src.h = spriteHeight;

					dest.x = sprt.pos.x;
					dest.y = sprt.pos.y;
					dest.w = spriteHeight * Game::spriteScale;
					dest.h = spriteHeight * Game::spriteScale;
					if (entity->entType == EntityType::Character) {
						SDL_RenderTextureRotated(Graphics::Renderer, (sprt.anim)->animTextures[textIdx], &src, &dest, 0, nullptr, SDL_FLIP_HORIZONTAL);
					}
					else {
						SDL_RenderTexture(Graphics::Renderer, (sprt.anim)->animTextures[textIdx], &src, &dest);
					}
				}
			}
			/*
			if (currentScreen->scrnType == ScreenType::BattleOn) {
				std::vector<Sprite>& sprites = currentScreen->getSpriteEntries();
				SDL_FRect src{};
				SDL_FRect dest{};

				
				
			}
			*/
			/*
			if (currentScreen->scrnType == ScreenType::VictoryUpgradeWeapon) {

				// current player wpn sprite pos
				std::vector<Sprite>& sprites = currentScreen->getSpriteEntries();
				Sprite& sprt = sprites[0];
				SDL_FRect src{};
				SDL_FRect dest{};

				uint8_t curWpn = Game::playerCharacter->wpnIdx;
				src.x = sprt.txtRs->dims.y * curWpn;
				src.y = 0;
				src.w = sprt.txtRs->dims.y;
				src.h = sprt.txtRs->dims.y;

				dest.x = sprt.pos.x;
				dest.y = sprt.pos.y;
				dest.w = sprt.txtRs->dims.y;
				dest.h = sprt.txtRs->dims.y;

				SDL_Texture* txt = sprt.txtRs->txt;
				SDL_RenderTexture(Graphics::Renderer, txt, &src, &dest);

				// new wpn sprite pos

				curWpn = static_cast<uint8_t>(Game::newWeapon->wpnType);

				Sprite& sprt2 = sprites[1];
				src.x = sprt2.txtRs->dims.y * curWpn;
				src.y = 0;
				src.w = sprt2.txtRs->dims.y;
				src.h = sprt2.txtRs->dims.y;

				dest.x = sprt2.pos.x;
				dest.y = sprt2.pos.y;
				dest.w = sprt2.txtRs->dims.y;
				dest.h = sprt2.txtRs->dims.y;

				txt = sprt2.txtRs->txt;
				SDL_RenderTexture(Graphics::Renderer, txt, &src, &dest);
			}
			*/
		}
	}

	void UpdateHealthbars() {

		playerHealthBar.h = Game::playerCharacter->health * UI::defHealthBarHeight / Game::playerCurrRoundMaxHealth;
		mnstrHealthBar.h = Game::randomMonster->health * UI::defHealthBarHeight / Game::mnstrCurrRoundMaxHealth;

		if (currentScreen->scrnType == ScreenType::BattleOn) {
			const std::vector<TextEntry>& textEntries = (*currentScreen).getTextEntries();

			playerHealthBar.x = textEntries[7].pos.x + 12;
			playerHealthBar.y = textEntries[7].pos.y - playerHealthBar.h;

			mnstrHealthBar.x = textEntries[8].pos.x + 12;
			mnstrHealthBar.y = textEntries[8].pos.y - mnstrHealthBar.h;

		}
	}
}
//UI

Animation* Anim::SetEntityAnimationByAnimType(Entity* entity, const AnimType& animTyp) {
	const AnimInfo* animInfo = nullptr;
	Animation* entityAnimations = nullptr;

	// find out what the entity is and get its AnimInfo
	if (entity->entType == EntityType::Character) {
		entityAnimations = playerAnimations;
		uint8_t classTp = static_cast<uint8_t>(Game::playerCharacter->classType);
		animInfo = &(*defPlayerAnimInfos)[classTp];
	}
	else if (entity->entType == EntityType::Monster) {
		entityAnimations = monsterAnimations;
		uint8_t classTp = static_cast<uint8_t>(Game::randomMonster->classType);
		animInfo = &(*defMonsterAnimInfos)[classTp];
	}

	entityAnimations->currentFrame = 0;
	entityAnimations->framesElapsed = 0;
	entityAnimations->bAnimHasEnded = false;

	switch (animTyp) {
		case AnimType::Attack: {
			// set attacker animations
			uint8_t idx = SDL_rand(3);
			entityAnimations->currentAnimTxtIdx = idx;
			entityAnimations->bIsLooping = false;
			entityAnimations->frameCount = animInfo->frameCounts[idx];

			bIsActiveAnimationEnded = false;
		}break;

		case AnimType::Idle: {
			entityAnimations->currentAnimTxtIdx = 3; // set idle animation texture
			entityAnimations->bIsLooping = true;
			entityAnimations->frameCount = animInfo->frameCounts[3];
		}break; 

		case AnimType::Hurt: {
			entityAnimations->currentAnimTxtIdx = 4; // set death animation texture
			entityAnimations->bIsLooping = true;
			entityAnimations->frameCount = animInfo->frameCounts[4];
		}break; 

		case AnimType::Dead: {
			entityAnimations->currentAnimTxtIdx = 5; // set death animation texture
			entityAnimations->bIsLooping = false;
			entityAnimations->frameCount = animInfo->frameCounts[5];

			bIsActiveAnimationEnded = false;
		}break; 

		default: break;
	}
	return entityAnimations;
}

void Anim::SetAnimationsByTypes() {
	attackerEntityAnimations = SetEntityAnimationByAnimType(Game::attacker, AnimType::Attack);

	if (Game::playerCharacter->health == 0 || Game::randomMonster->health == 0) {
		victimEntityAnimations = SetEntityAnimationByAnimType(Game::victim, AnimType::Dead);
	}
	else{
		victimEntityAnimations = SetEntityAnimationByAnimType(Game::victim, AnimType::Idle);
	}
	
	Game::UpdateSpritePositions();
}

bool InitializeGraphics() {
	
	// init SDL
	bool isInited = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_CAMERA);

	if (!isInited) {
		spdlog::error("SDL failed to initialize.\n");
		return false;
	}

#ifdef ALWAYS_ON_TOP
	SDL_Window* wndw = SDL_CreateWindow("Lesta Test Game", Graphics::TargetWindowWidth, Graphics::TargetWindowHeight,
		SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS |
		SDL_WINDOW_KEYBOARD_GRABBED| SDL_WINDOW_ALWAYS_ON_TOP );//| SDL_WINDOW_RESIZABLE);
#else
	SDL_Window* wndw = SDL_CreateWindow("Lesta Test Game", Graphics::TargetWindowWidth, Graphics::TargetWindowHeight,
		SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_KEYBOARD_GRABBED);
#endif
#undef ALWAYS_ON_TOP


	if (!wndw) {
		spdlog::error("SDL failed to create window.\n");
		return false;
	}

	Graphics::Window = wndw;

	SDL_Renderer* rnder = SDL_CreateRenderer(wndw, NULL);

	if (!rnder) {
		spdlog::error("SDL failed to create renderer.\n");
		return false;
	}

	Graphics::Renderer = rnder;

	//SDL_SetWindowFullscreen(Graphics::Window, true);

	if (!TTF_Init()) {
		spdlog::error("Failed to initialize TFF.\n");
		return false;
	}
	

	return Game::bGameIsRunning = true;
}

void LoadResources()
{
	// load font for text
	Resources::buttonsFont			= TTF_OpenFont("resources/fonts/Norse-Bold.ttf", UI::buttonsFontSize);
	Resources::statsFont			= TTF_OpenFont("resources/fonts/Lorenzo Sans Regular.ttf", UI::statsFontSize);
	Resources::descLargeTextFont	= TTF_OpenFont("resources/fonts/Norse-Bold.ttf", UI::descLargeTextFontSize);
	Resources::descSmallTextFont	= TTF_OpenFont("resources/fonts/Norse-Bold.ttf", UI::descSmallTextFontSize);
	Resources::descAboutTextFont	= TTF_OpenFont("resources/fonts/Lorenzo Sans Regular.ttf", UI::descAboutTextFontSize);
	Resources::gameTitleFont		= TTF_OpenFont("resources/fonts/Norse-Bold.ttf", UI::gameTitleFontSize);
	Resources::statsFontSmall		= TTF_OpenFont("resources/fonts/Lorenzo Sans Regular.ttf", UI::statsFontSmallSize);


	(*Resources::fonts)[0] = Resources::buttonsFont;
	(*Resources::fonts)[1] = Resources::statsFont;
	(*Resources::fonts)[2] = Resources::descLargeTextFont;
	(*Resources::fonts)[3] = Resources::descSmallTextFont;
	(*Resources::fonts)[4] = Resources::descAboutTextFont;
	(*Resources::fonts)[5] = Resources::gameTitleFont;
	(*Resources::fonts)[6] = Resources::statsFontSmall;



	// load button textures and push them in ButtonType structure instance
	SDL_Texture* txt;
	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/ui/Button_01.png");
	(*UI::buttonTypes).push_back_entry(txt, 370, 100);// button texture, width and height of a button

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/ui/Button_02.png");
	(*UI::buttonTypes).push_back_entry(txt, 220, 100);

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/ui/Button_01.png");
	(*UI::buttonTypes).push_back_entry(txt, 100, 100);

	// load screen background textures and set them in respective screens
	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/main_menu.png");

	(*UI::screens)[0]->backgroundImage = txt;

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/character_selection.png");
	(*UI::screens)[1]->backgroundImage = txt;
	
	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/victory.png");
	(*UI::screens)[3]->backgroundImage = txt;

	(*UI::screens)[4]->backgroundImage = txt;

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/defeated.png");
	(*UI::screens)[5]->backgroundImage = txt;

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/game_completed.png");
	(*UI::screens)[6]->backgroundImage = txt;

	// load background images for battle screen
	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/battle/castle.png");
	UI::rndBackground->Images.push_back(txt);

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/battle/dead forest.png");
	UI::rndBackground->Images.push_back(txt);

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/battle/terrace.png");
	UI::rndBackground->Images.push_back(txt);

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/background/battle/throne room.png");
	UI::rndBackground->Images.push_back(txt);

	// load other ui textures

	txt = IMG_LoadTexture(Graphics::Renderer, "resources/images/ui/weapon_icons.png");
	TextureResourceEntry& rs = (*Resources::ui_textures)[0];
	rs.txt = txt;
	rs.dims = { 576, 96 };
}

void InitializeLocalization() {
	BtnFuncitons::SetLocalizationEn();
}

void InitializeScreens()
{
	// set button type indexes, used later to access button dimentions,
	// bind callback functions to button funciton pointers,
	// initialize all screens and assign them corresponding buttons

	//main menu and generic buttons
	Button PlayGame;
	PlayGame.btnTypeIdx = 0;
	PlayGame.fnToInvoke = BtnFuncitons::GoTo_CharacterSelection_Screen;
	PlayGame.btnTextId = 1;

	Button Settings;
	Settings.btnTypeIdx = 0;
	Settings.fnToInvoke = BtnFuncitons::GoTo_Settings_Screen;
	Settings.btnTextId = 2;

	Button ExitGame;
	ExitGame.btnTypeIdx = 0;
	ExitGame.fnToInvoke = BtnFuncitons::ExitGame;
	ExitGame.btnTextId = 3;

	Button About;
	About.btnTypeIdx = 1;
	About.fnToInvoke = BtnFuncitons::GoTo_About_Screen;
	About.btnTextId = 4;

	Button ExitToMainMenu;
	ExitToMainMenu.btnTypeIdx = 0;
	ExitToMainMenu.fnToInvoke = BtnFuncitons::GoTo_MainMenu_Screen;
	ExitToMainMenu.btnTextId = 9;

	// character selection screen buttons
	Button SelectCharacter1;
	SelectCharacter1.btnTypeIdx = 0;
	SelectCharacter1.bIsTogglable = true;
	SelectCharacter1.fnToInvoke = BtnFuncitons::SelectCharacter_Outlaw;
	SelectCharacter1.btnTextId = 6;

	Button SelectCharacter2;
	SelectCharacter2.btnTypeIdx = 0;
	SelectCharacter2.bIsTogglable = true;
	SelectCharacter2.fnToInvoke = BtnFuncitons::SelectCharacter_Warrior;
	SelectCharacter2.btnTextId = 7;

	Button SelectCharacter3;
	SelectCharacter3.btnTypeIdx = 0;
	SelectCharacter3.bIsTogglable = true;
	SelectCharacter3.fnToInvoke = BtnFuncitons::SelectCharacter_Barbarian;
	SelectCharacter3.btnTextId = 8;

	Button ConfirmAndContinue;
	ConfirmAndContinue.btnTypeIdx = 0;
	ConfirmAndContinue.bIsActive = false;
	ConfirmAndContinue.fnToInvoke = BtnFuncitons::ConfirmCharSelection_And_GoTo_BattleOn_Screen;
	ConfirmAndContinue.btnTextId = 5;

	// battle screen debug buttons, will not be present in final game
	Button Dbg_Victory;
	Dbg_Victory.btnTypeIdx = 1;
	Dbg_Victory.fnToInvoke = BtnFuncitons::Dbg_GoTo_VictoryWpnUpgrade_Screen;
	Dbg_Victory.btnTextId = 14;

	Button Dbg_Defeated;
	Dbg_Defeated.btnTypeIdx = 1;
	Dbg_Defeated.fnToInvoke = BtnFuncitons::Dbg_GoTo_Defeated_Screen;
	Dbg_Defeated.btnTextId = 15;

	Button Dbg_Completed;
	Dbg_Completed.btnTypeIdx = 1;
	Dbg_Completed.fnToInvoke = BtnFuncitons::Dbg_GoTo_Completed_Screen;
	Dbg_Completed.btnTextId = 16;

	//victory screen, equip/dismiss new weapon buttons
	Button EquipNewWeapon;
	EquipNewWeapon.btnTypeIdx = 0;
	EquipNewWeapon.fnToInvoke = BtnFuncitons::EquipNewWeapon_And_GoTo_CharacterUpgrade_Screen;
	EquipNewWeapon.btnTextId = 10;

	Button DismissNewWeapon;
	DismissNewWeapon.btnTypeIdx = 0;
	DismissNewWeapon.fnToInvoke = BtnFuncitons::DismissNewWeapon_And_GoTo_CharacterUpgrade_Screen;
	DismissNewWeapon.btnTextId = 11;

	//victory screen, select to upgrade character or continue without multiclass, buttons
	Button SelectNewCharacter;
	SelectNewCharacter.btnTypeIdx = 0;
	SelectNewCharacter.fnToInvoke = BtnFuncitons::GoTo_CharacterSelection_Screen;
	SelectNewCharacter.btnTextId = 12;

	Button ContinueWithCurrent;
	ContinueWithCurrent.btnTypeIdx = 0;
	ContinueWithCurrent.fnToInvoke = BtnFuncitons::ContinueWithoutMulticl_And_GoTo_BattleOn_Screen;
	ContinueWithCurrent.btnTextId = 13;

	// mian menu screen
	{
		(*UI::screens)[0]->scrnType = ScreenType::MainMenu;
		(*UI::screens)[0]->push_back_button(PlayGame);
		(*UI::screens)[0]->push_back_button(Settings);
		(*UI::screens)[0]->push_back_button(ExitGame);
		(*UI::screens)[0]->push_back_button(About);

		TextEntry GameName;
		GameName.textIdx = 4;
		GameName.fontIdx = 5;
		(*UI::screens)[0]->push_back_text_entry(GameName);
	}

	// character selection screen
	{
		(*UI::screens)[1]->scrnType = ScreenType::CharacterSelection;
		(*UI::screens)[1]->push_back_button(SelectCharacter1);
		(*UI::screens)[1]->push_back_button(SelectCharacter2);
		(*UI::screens)[1]->push_back_button(SelectCharacter3);
		(*UI::screens)[1]->push_back_button(ConfirmAndContinue);
		(*UI::screens)[1]->push_back_button(ExitToMainMenu);

		TextEntry SelYourChar;
		SelYourChar.textIdx = 5;
		SelYourChar.fontIdx = 2;

		(*UI::screens)[1]->push_back_text_entry(SelYourChar);
	}

	Sprite CharacterSprite;
	CharacterSprite.anim = Anim::playerAnimations;

	Sprite MonsterSprite;
	MonsterSprite.anim = Anim::monsterAnimations;

	TextEntry playerWpnSpritePos;
	playerWpnSpritePos.bIsVisible = false;

	// battle screen
	{
		Sprite battleOnUi;
		battleOnUi.bIsSpriteAnimated = false;
		battleOnUi.txtRs = &(*Resources::ui_textures)[0];

		(*UI::screens)[2]->scrnType = ScreenType::BattleOn;
		/*
		(*UI::screens)[2]->push_back_button(Dbg_Victory);
		(*UI::screens)[2]->push_back_button(Dbg_Defeated);
		(*UI::screens)[2]->push_back_button(Dbg_Completed);
		(*UI::screens)[2]->push_back_button(PlayGame);
		*/

		(*UI::screens)[2]->push_back_sprite(CharacterSprite);
		(*UI::screens)[2]->push_back_sprite(MonsterSprite);

		// player wpn icon position
		(*UI::screens)[2]->push_back_text_entry(playerWpnSpritePos); // 0

		TextEntry playerStatsPos;
		playerStatsPos.bIsVisible = false;
		(*UI::screens)[2]->push_back_text_entry(playerStatsPos); // 1

		TextEntry monsterStatsPos;
		monsterStatsPos.bIsVisible = false;
		(*UI::screens)[2]->push_back_text_entry(monsterStatsPos); // 2

		// hero name text
		TextEntry playerNamePos;
		playerNamePos.textIdx = 30;
		playerNamePos.fontIdx = 3;
		playerNamePos.bIsVisible = true;
		(*UI::screens)[2]->push_back_text_entry(playerNamePos); // 3

		// monster name text
		TextEntry monsterNamePos;
		monsterNamePos.textIdx = 33;
		monsterNamePos.fontIdx = 3;
		monsterNamePos.bIsVisible = true;
		(*UI::screens)[2]->push_back_text_entry(monsterNamePos); // 4

		// character level text
		TextEntry charLevel;
		charLevel.textIdx = 28;
		charLevel.fontIdx = 1;
		charLevel.bIsVisible = true;
		(*UI::screens)[2]->push_back_text_entry(charLevel); // 5

		// round text
		TextEntry roundNum;
		roundNum.textIdx = 26;
		roundNum.fontIdx = 3;
		roundNum.bIsVisible = true;
		(*UI::screens)[2]->push_back_text_entry(roundNum); // 6

		// player health text
		TextEntry playerHealth;
		playerHealth.textIdx = 29;
		playerHealth.fontIdx = 1;
		playerHealth.bIsVisible = true;
		(*UI::screens)[2]->push_back_text_entry(playerHealth); // 7

		// monster health text
		TextEntry monsterHealth;
		monsterHealth.textIdx = 29;
		monsterHealth.fontIdx = 1;
		monsterHealth.bIsVisible = true;
		(*UI::screens)[2]->push_back_text_entry(monsterHealth); // 8
	}

	TextEntry Victory;
	Victory.textIdx = 6;
	Victory.fontIdx = 2;

	//victory screen: equip or dismiss new weapon
	{
		(*UI::screens)[3]->scrnType = ScreenType::VictoryUpgradeWeapon;
		(*UI::screens)[3]->push_back_button(EquipNewWeapon);
		(*UI::screens)[3]->push_back_button(DismissNewWeapon);
		(*UI::screens)[3]->push_back_button(ExitToMainMenu);

		TextEntry SelectWpnOrDismiss;
		SelectWpnOrDismiss.textIdx = 2;
		SelectWpnOrDismiss.fontIdx = 3;

		(*UI::screens)[3]->push_back_text_entry(Victory);
		(*UI::screens)[3]->push_back_text_entry(SelectWpnOrDismiss);
		(*UI::screens)[3]->push_back_text_entry(playerWpnSpritePos);

		TextEntry newWpnSpritePos;
		newWpnSpritePos.bIsVisible = false;
		(*UI::screens)[3]->push_back_text_entry(newWpnSpritePos);
	}

	// victory screen: select new character or continue with current
	{
		(*UI::screens)[4]->scrnType = ScreenType::VictorySelectChar;
		(*UI::screens)[4]->push_back_button(SelectNewCharacter);
		(*UI::screens)[4]->push_back_button(ContinueWithCurrent);
		(*UI::screens)[4]->push_back_button(ExitToMainMenu);

		TextEntry SelectCharUpgrOrCont;
		SelectCharUpgrOrCont.textIdx = 3;
		SelectCharUpgrOrCont.fontIdx = 3;

		(*UI::screens)[4]->push_back_text_entry(Victory);
		(*UI::screens)[4]->push_back_text_entry(SelectCharUpgrOrCont);

	}

	TextEntry DefeatedCompletedTxt;
	DefeatedCompletedTxt.textIdx = 11;
	DefeatedCompletedTxt.fontIdx = 3;

	// player was defeated screen
	{
		(*UI::screens)[5]->scrnType = ScreenType::Defeated;
		(*UI::screens)[5]->push_back_button(ExitToMainMenu);
		(*UI::screens)[5]->push_back_button(ExitGame);

		TextEntry Defeated;
		Defeated.textIdx = 7;
		Defeated.fontIdx = 2;

		TextEntry DefeatedTxt;
		DefeatedTxt.textIdx = 8;
		DefeatedTxt.fontIdx = 3;

		(*UI::screens)[5]->push_back_text_entry(Defeated);
		(*UI::screens)[5]->push_back_text_entry(DefeatedTxt);
		(*UI::screens)[5]->push_back_text_entry(DefeatedCompletedTxt);

	}

	//game completed screen
	{
		(*UI::screens)[6]->scrnType = ScreenType::GameCompleted;
		(*UI::screens)[6]->push_back_button(ExitToMainMenu);
		(*UI::screens)[6]->push_back_button(ExitGame);

		TextEntry GameCompleted;
		GameCompleted.textIdx = 9;
		GameCompleted.fontIdx = 2;

		TextEntry GameCompletedTxt;
		GameCompletedTxt.textIdx = 10;
		GameCompletedTxt.fontIdx = 3;

		(*UI::screens)[6]->push_back_text_entry(GameCompleted);
		(*UI::screens)[6]->push_back_text_entry(GameCompletedTxt);
		(*UI::screens)[6]->push_back_text_entry(DefeatedCompletedTxt);

	}

	// settings screen
	{
		Button SetLocalizationRus;
		SetLocalizationRus.btnTypeIdx = 1;
		SetLocalizationRus.bIsTogglable = 1;
		SetLocalizationRus.fnToInvoke = BtnFuncitons::SetLocalizationRus;
		SetLocalizationRus.btnTextId = 17;

		Button SetLocalizationEn;
		SetLocalizationEn.btnTypeIdx = 1;
		SetLocalizationEn.bIsTogglable = 1;
		SetLocalizationEn.bIsToggledOn = 1;
		SetLocalizationEn.fnToInvoke = BtnFuncitons::SetLocalizationEn;
		SetLocalizationEn.btnTextId = 18;

		(*UI::screens)[7]->scrnType = ScreenType::Settings;
		(*UI::screens)[7]->push_back_button(ExitToMainMenu);
		(*UI::screens)[7]->push_back_button(SetLocalizationRus);
		(*UI::screens)[7]->push_back_button(SetLocalizationEn);

		TextEntry CurLanguageTxt;
		CurLanguageTxt.textIdx = 12;
		CurLanguageTxt.fontIdx = 0;

		/*
		TextEntry Dbg_SpacebarToEnd;
		Dbg_SpacebarToEnd.textIdx = 0;
		*/

		(*UI::screens)[7]->push_back_text_entry(CurLanguageTxt);
		//(*UI::screens)[7]->push_back_text_entry(Dbg_SpacebarToEnd);
	}

	// about screen, with info about the game
	{
		TextEntry AboutGameTxt;
		AboutGameTxt.textIdx = 1;
		AboutGameTxt.fontIdx = 4;

		(*UI::screens)[8]->scrnType = ScreenType::About;
		(*UI::screens)[8]->push_back_button(ExitToMainMenu);
		(*UI::screens)[8]->push_back_text_entry(AboutGameTxt);
	}

	//at the start of the game, set screen ptr to main menu screen
	UI::currentScreen = (*UI::screens)[0];
}

void UpdateButtonsAndUiTextPositions() {
		
	// here we set positions to all buttons in all screens, all button positions are relative 
	// to WindowWidth and WindowHeight
	// this function is ment to be called on ui initialization and on window rezie event
	
	// generic button type btnTypeIdx = 0
	uint16_t type_0_ButtonWidth		= UI::buttonTypes->getBtnDimsByTypeIdx(0).x;
	uint16_t type_0_ButtonHeight	= UI::buttonTypes->getBtnDimsByTypeIdx(0).y;

	// generic button type btnTypeIdx = 1
	uint16_t type_1_ButtonWidth		= UI::buttonTypes->getBtnDimsByTypeIdx(1).x;
	uint16_t type_1_ButtonHeight	= UI::buttonTypes->getBtnDimsByTypeIdx(1).y;

	// generic button type btnTypeIdx = 2
	//uint16_t type_2_ButtonWidth	= buttonType.getBtnDimsByTypeIdx(2).x;
	//uint16_t type_2_ButtonHeight	= buttonType.getBtnDimsByTypeIdx(2).y;


	float mainMenu_Btns_HorizontalPos		= Graphics::WindowWidth / 2 - type_0_ButtonWidth / 2;
	float mainMenu_Btns_VerticalPosStart	= Graphics::WindowHeight / 2;

	float w = Graphics::WindowWidth;
	float h = Graphics::WindowHeight;

	float mainMenu_Btns_VerticalPadding = type_0_ButtonHeight + Graphics::WindowHeight * 0.01f;
	
	float exitToMainMenu_Btn_HorizontalPos = Graphics::WindowWidth * 0.01f;
	float exitToMainMenu_Btn_VerticalPos = Graphics::WindowHeight - (type_0_ButtonHeight + Graphics::WindowWidth * 0.01f);

	// main menu button positons
	{
		std::vector<Button>& MainMenu_btns = (*UI::screens)[0]->getButtons();

		//PlayGame
		MainMenu_btns[0].pos.x = mainMenu_Btns_HorizontalPos;
		MainMenu_btns[0].pos.y = mainMenu_Btns_VerticalPosStart;

		//Settings
		MainMenu_btns[1].pos.x = mainMenu_Btns_HorizontalPos;
		MainMenu_btns[1].pos.y = mainMenu_Btns_VerticalPosStart + mainMenu_Btns_VerticalPadding;

		//ExitGame
		MainMenu_btns[2].pos.x = mainMenu_Btns_HorizontalPos;
		MainMenu_btns[2].pos.y = mainMenu_Btns_VerticalPosStart + mainMenu_Btns_VerticalPadding * 2;

		//About
		MainMenu_btns[3].pos.x = exitToMainMenu_Btn_HorizontalPos;
		MainMenu_btns[3].pos.y = exitToMainMenu_Btn_VerticalPos;

		std::vector<TextEntry>& MainMenu_txt = (*UI::screens)[0]->getTextEntries();
		float gameTitle_txt_HorizontalPos = Graphics::WindowWidth * 0.08f;
		float gameTitle_txt_VerticalPos = Graphics::WindowHeight * 0.10f;

		//text: game title
		MainMenu_txt[0].pos.x = gameTitle_txt_HorizontalPos;
		MainMenu_txt[0].pos.y = gameTitle_txt_VerticalPos;
	}

	float charSelection_Btns_HorizontalPos = Graphics::WindowWidth  - (type_0_ButtonWidth + Graphics::WindowWidth * 0.01f);
	float charSelection_Btns_VerticalPosStart = Graphics::WindowHeight / 4;
	
	// character selection screen button positons
	{
		std::vector<Button>& CharSelection_btns = (*UI::screens)[1]->getButtons();

		//SelectCharacter1
		CharSelection_btns[0].pos.x = charSelection_Btns_HorizontalPos;
		CharSelection_btns[0].pos.y = charSelection_Btns_VerticalPosStart;

		//SelectCharacter2
		CharSelection_btns[1].pos.x = charSelection_Btns_HorizontalPos;
		CharSelection_btns[1].pos.y = charSelection_Btns_VerticalPosStart + mainMenu_Btns_VerticalPadding;

		//SelectCharacter3
		CharSelection_btns[2].pos.x = charSelection_Btns_HorizontalPos;
		CharSelection_btns[2].pos.y = charSelection_Btns_VerticalPosStart + mainMenu_Btns_VerticalPadding * 2;

		//ConfirmAndContinue
		CharSelection_btns[3].pos.x = charSelection_Btns_HorizontalPos;
		CharSelection_btns[3].pos.y = exitToMainMenu_Btn_VerticalPos;

		//ExitToMainMenu
		CharSelection_btns[4].pos.x = exitToMainMenu_Btn_HorizontalPos;
		CharSelection_btns[4].pos.y = exitToMainMenu_Btn_VerticalPos;

		std::vector<TextEntry>& CharSelection_txt = (*UI::screens)[1]->getTextEntries();
		float selectChar_txt_HorizontalPos = Graphics::WindowWidth * 0.12f;
		float selectChar_txt_VerticalPos = Graphics::WindowHeight * 0.04f;
		// text: select your character
		CharSelection_txt[0].pos.x = selectChar_txt_HorizontalPos;
		CharSelection_txt[0].pos.y = selectChar_txt_VerticalPos;
	}

	// battleon screen button positons 
	{
		float dbg_Btns_HorizontalPos = Graphics::WindowWidth  - type_1_ButtonWidth * 3;
		//float dbg_Btns_VerticalPos = Graphics::WindowHeight - type_1_ButtonHeight - 300;
		//float dbg_Btns_VerticalPos = 0;
		float dbg_Btns_VerticalPos = Graphics::WindowHeight - type_1_ButtonHeight;


		/*
		std::vector<Button>& BattleOn_btns = (*UI::screens)[2]->getButtons();
		//Dbg_Victory
		BattleOn_btns[0].pos.x = dbg_Btns_HorizontalPos;
		BattleOn_btns[0].pos.y = dbg_Btns_VerticalPos;

		//Dbg_Defeated
		BattleOn_btns[1].pos.x = dbg_Btns_HorizontalPos + type_1_ButtonWidth;
		BattleOn_btns[1].pos.y = dbg_Btns_VerticalPos;

		//Dbg_Completed
		BattleOn_btns[2].pos.x = dbg_Btns_HorizontalPos + type_1_ButtonWidth*2;
		BattleOn_btns[2].pos.y = dbg_Btns_VerticalPos;

		//PlayGame
		BattleOn_btns[3].pos.x = 0;
		BattleOn_btns[3].pos.y = dbg_Btns_VerticalPos;
		*/

		
		float heroChar_StatsHorizontalPos	= Graphics::WindowWidth * 0.83f;
		float monster_StatsHorizontalPos	= Graphics::WindowWidth * 0.03f;
		float StatsVerticalPos = Graphics::WindowHeight * 0.13f;
		std::vector<TextEntry>& BattleOn_StatsLocations = (*UI::screens)[2]->getTextEntries();
		
		// curr player wpn icon 
		BattleOn_StatsLocations[0].pos.x = Graphics::WindowWidth / 2;
		BattleOn_StatsLocations[0].pos.y = Graphics::WindowHeight * 0.77f;

		// player stats pos 
		BattleOn_StatsLocations[1].pos.x = heroChar_StatsHorizontalPos;
		BattleOn_StatsLocations[1].pos.y = StatsVerticalPos;

		// monster stats pos 
		BattleOn_StatsLocations[2].pos.x = monster_StatsHorizontalPos;
		BattleOn_StatsLocations[2].pos.y = StatsVerticalPos;

		float heroChar_NameHorizontalPos = Graphics::WindowWidth - Graphics::WindowWidth / 6;
		float monster_NameHorizontalPos = Graphics::WindowWidth / 32;
		float NamesVerticalPos = Graphics::WindowHeight * 0.015f;

		// player name text pos 
		BattleOn_StatsLocations[3].pos.x = heroChar_NameHorizontalPos;
		BattleOn_StatsLocations[3].pos.y = NamesVerticalPos;

		// monster name text pos 
		BattleOn_StatsLocations[4].pos.x = monster_NameHorizontalPos;
		BattleOn_StatsLocations[4].pos.y = NamesVerticalPos;

		// char level text pos
		BattleOn_StatsLocations[5].pos.x = heroChar_NameHorizontalPos;
		BattleOn_StatsLocations[5].pos.y = NamesVerticalPos + Graphics::WindowHeight * 0.07f;

		// round number text pos
		BattleOn_StatsLocations[6].pos.x = Graphics::WindowWidth * 0.45f;
		BattleOn_StatsLocations[6].pos.y = Graphics::WindowHeight * 0.07f;

		float charMnstr_Health_verticalPos = Graphics::WindowHeight * 0.71f;

		// character health text pos
		BattleOn_StatsLocations[7].pos.x = Graphics::WindowWidth * 0.73f;
		BattleOn_StatsLocations[7].pos.y = charMnstr_Health_verticalPos;

		// monster health text pos
		BattleOn_StatsLocations[8].pos.x = Graphics::WindowWidth * 0.203f;
		BattleOn_StatsLocations[8].pos.y = charMnstr_Health_verticalPos;

		float heroChar_HorizontalPos = Graphics::WindowWidth / 2 + 130;
		float heroChar_VerticalPos = Graphics::WindowHeight / 2 + Graphics::WindowHeight / 8;
		float monster_HorizontalPos = Graphics::WindowWidth / 2 - 130;

		std::vector<Sprite>& BattleOn_sprites = (*UI::screens)[2]->getSpriteEntries();
		// hero sprite
		BattleOn_sprites[0].pos.x = heroChar_HorizontalPos;
		BattleOn_sprites[0].pos.y = heroChar_VerticalPos;
		// monster sprite
		BattleOn_sprites[1].pos.x = monster_HorizontalPos;
		BattleOn_sprites[1].pos.y = heroChar_VerticalPos;
		
	}
	
	float victoryUpgradeWeapon_Equip_Btn_HorizontalPos = Graphics::WindowWidth / 2 + Graphics::WindowWidth / 32;
	float victoryUpgradeWeapon_Dismiss_Btn_HorizontalPos = Graphics::WindowWidth / 2 
		- (type_0_ButtonWidth + Graphics::WindowWidth / 32);
	float victoryUpgradeWeapon_Btns_VerticalPos = Graphics::WindowHeight * 0.68f;


	float weaponUpgrade_txt_HorizontalPos = Graphics::WindowWidth * 0.05f;
	float weaponUpgrade_txt_VerticalPos = Graphics::WindowHeight * 0.03f;

	// victory (weapon selection) screen button positons 
	{
		std::vector<Button>& VictoryUpgradeWeapon_btns = (*UI::screens)[3]->getButtons();
		//EquipNewWeapon
		VictoryUpgradeWeapon_btns[0].pos.x = victoryUpgradeWeapon_Equip_Btn_HorizontalPos;
		VictoryUpgradeWeapon_btns[0].pos.y = victoryUpgradeWeapon_Btns_VerticalPos;

		//DismissNewWeapon
		VictoryUpgradeWeapon_btns[1].pos.x = victoryUpgradeWeapon_Dismiss_Btn_HorizontalPos;
		VictoryUpgradeWeapon_btns[1].pos.y = victoryUpgradeWeapon_Btns_VerticalPos;

		//ExitToMainMenu
		VictoryUpgradeWeapon_btns[2].pos.x = exitToMainMenu_Btn_HorizontalPos;
		VictoryUpgradeWeapon_btns[2].pos.y = exitToMainMenu_Btn_VerticalPos;

		std::vector<TextEntry>& weaponUpgrade_txt = (*UI::screens)[3]->getTextEntries();
		//float wpnSelection_txt_HorizontalPos = Graphics::WindowWidth * 0.1f;

		// text: upgrade wpn or dismiss
		weaponUpgrade_txt[0].pos.x = weaponUpgrade_txt_HorizontalPos;
		weaponUpgrade_txt[0].pos.y = weaponUpgrade_txt_VerticalPos;

		weaponUpgrade_txt[1].pos.x = victoryUpgradeWeapon_Dismiss_Btn_HorizontalPos - 30;
		weaponUpgrade_txt[1].pos.y = victoryUpgradeWeapon_Btns_VerticalPos - 70;

	
		float weaponUpgrade_sprites_VerticalPos = victoryUpgradeWeapon_Btns_VerticalPos - 270;
		float weaponUpgrade_Dismiss_sprite_HorizontalPos = Graphics::WindowWidth * 0.23f;
		float weaponUpgrade_Equip_sprite_HorizontalPos = Graphics::WindowWidth * 0.65f;
		
		// dismiss new weapon icon pos
		weaponUpgrade_txt[2].pos.x = weaponUpgrade_Dismiss_sprite_HorizontalPos;
		weaponUpgrade_txt[2].pos.y = weaponUpgrade_sprites_VerticalPos;
		// equip new weapon icon pos
		weaponUpgrade_txt[3].pos.x = weaponUpgrade_Equip_sprite_HorizontalPos;
		weaponUpgrade_txt[3].pos.y = weaponUpgrade_sprites_VerticalPos;
		

	}

	// victory (select new char) screen button positons 
	{
		std::vector<Button>& VictorySelectChar_btns = (*UI::screens)[4]->getButtons();
		//SelectNewCharacter
		VictorySelectChar_btns[0].pos.x = victoryUpgradeWeapon_Dismiss_Btn_HorizontalPos;
		VictorySelectChar_btns[0].pos.y = victoryUpgradeWeapon_Btns_VerticalPos;

		//ContinueWithCurrent
		VictorySelectChar_btns[1].pos.x = victoryUpgradeWeapon_Equip_Btn_HorizontalPos;
		VictorySelectChar_btns[1].pos.y = victoryUpgradeWeapon_Btns_VerticalPos;

		//ExitToMainMenu
		VictorySelectChar_btns[2].pos.x = exitToMainMenu_Btn_HorizontalPos;
		VictorySelectChar_btns[2].pos.y = exitToMainMenu_Btn_VerticalPos;

		std::vector<TextEntry>& charUpgrade_txt = (*UI::screens)[4]->getTextEntries();

		// text: upgrade wpn or dismiss
		charUpgrade_txt[0].pos.x = weaponUpgrade_txt_HorizontalPos;
		charUpgrade_txt[0].pos.y = weaponUpgrade_txt_VerticalPos;

		charUpgrade_txt[1].pos.x = victoryUpgradeWeapon_Dismiss_Btn_HorizontalPos;
		charUpgrade_txt[1].pos.y = victoryUpgradeWeapon_Btns_VerticalPos - 100;
	}

	float defeated_completed_txt_HorizontalPos = Graphics::WindowWidth * 0.05f;
	float defeated_completed_txt_VerticalPos = Graphics::WindowHeight * 0.15f;
	float defeated_completed_txt2_HorizontalPos = Graphics::WindowWidth / 2 - 330;

	// defeated screen button positons 
	{
			std::vector<Button>& Defeated_btns = (*UI::screens)[5]->getButtons();
			//ExitToMainMenu
			Defeated_btns[0].pos.x = charSelection_Btns_HorizontalPos;
			Defeated_btns[0].pos.y = exitToMainMenu_Btn_VerticalPos;

			//ExitGame
			Defeated_btns[1].pos.x = exitToMainMenu_Btn_HorizontalPos;
			Defeated_btns[1].pos.y = exitToMainMenu_Btn_VerticalPos;

			std::vector<TextEntry>& defeated_txt = (*UI::screens)[5]->getTextEntries();

			// text: Defeated
			defeated_txt[0].pos.x = defeated_completed_txt_HorizontalPos;
			defeated_txt[0].pos.y = defeated_completed_txt_VerticalPos;
			// text: Defeated description
			defeated_txt[1].pos.x = defeated_completed_txt_HorizontalPos;
			defeated_txt[1].pos.y = defeated_completed_txt_VerticalPos + 100;
			// text: exit to main menu and create new char
			defeated_txt[2].pos.x = defeated_completed_txt2_HorizontalPos;
			defeated_txt[2].pos.y = exitToMainMenu_Btn_VerticalPos - 70;
		}

	// GameCompleted screen button positons 
	{
			std::vector<Button>& GameCompleted_btns = (*UI::screens)[6]->getButtons();
			//ExitToMainMenu
			GameCompleted_btns[0].pos.x = charSelection_Btns_HorizontalPos;
			GameCompleted_btns[0].pos.y = exitToMainMenu_Btn_VerticalPos;

			//ExitGame
			GameCompleted_btns[1].pos.x = exitToMainMenu_Btn_HorizontalPos;
			GameCompleted_btns[1].pos.y = exitToMainMenu_Btn_VerticalPos;

			std::vector<TextEntry>& completed_txt = (*UI::screens)[6]->getTextEntries();

			// text: Game completed
			completed_txt[0].pos.x = defeated_completed_txt_HorizontalPos;
			completed_txt[0].pos.y = defeated_completed_txt_VerticalPos;
			// text: Game completed description
			completed_txt[1].pos.x = defeated_completed_txt_HorizontalPos;
			completed_txt[1].pos.y = defeated_completed_txt_VerticalPos + 100;
			// text: exit to main menu and create new char
			completed_txt[2].pos.x = defeated_completed_txt2_HorizontalPos;
			completed_txt[2].pos.y = exitToMainMenu_Btn_VerticalPos - 70;
	}

	// settings screen 
	{
			std::vector<Button>& Settings_btns = (*UI::screens)[7]->getButtons();
			Settings_btns[0].pos.x = exitToMainMenu_Btn_HorizontalPos;
			Settings_btns[0].pos.y = exitToMainMenu_Btn_VerticalPos;

			float Settings_localizRussian_HorizontalPos = Graphics::WindowWidth * 0.05f;
			float Settings_localizEnglish_HorizontalPos = Settings_localizRussian_HorizontalPos + type_1_ButtonWidth + 20;
			float Settings_localizButtons_VerticalPos = Graphics::WindowWidth * 0.05f;

			Settings_btns[1].pos.x = Settings_localizRussian_HorizontalPos;
			Settings_btns[1].pos.y = Settings_localizButtons_VerticalPos;

			Settings_btns[2].pos.x = Settings_localizEnglish_HorizontalPos;
			Settings_btns[2].pos.y = Settings_localizButtons_VerticalPos;

			std::vector<TextEntry>& settings_txt = (*UI::screens)[7]->getTextEntries();

			// text: Current Language
			settings_txt[0].pos.x = Settings_localizEnglish_HorizontalPos + (type_1_ButtonWidth + 10);
			settings_txt[0].pos.y = Settings_localizButtons_VerticalPos + 25;

	
	}

	// about 
	{
			std::vector<Button>& About_btns = (*UI::screens)[8]->getButtons();
			About_btns[0].pos.x = exitToMainMenu_Btn_HorizontalPos;
			About_btns[0].pos.y = exitToMainMenu_Btn_VerticalPos;

			float about_text_HorizontalPos = Graphics::WindowWidth * 0.03f;
			float about_text_VerticalPos = Graphics::WindowHeight * 0.05f;

			std::vector<TextEntry>& About_text = (*UI::screens)[8]->getTextEntries();
			About_text[0].pos.x = about_text_HorizontalPos;
			About_text[0].pos.y = about_text_VerticalPos;
	}
}

void ProcessPlayerInput() {

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {

			case SDL_EVENT_QUIT: {
				Game::bGameIsRunning = false;
			} break;

			case SDL_EVENT_KEY_DOWN: {
				if (event.key.key == SDLK_ESCAPE) Game::bGameIsRunning = false;
			} break;

			case SDL_EVENT_MOUSE_MOTION: {
				System::mouseCoords.x = event.motion.x;
				System::mouseCoords.y = event.motion.y;
			} break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN: {
				System::bMouseBtnDown = true;
			} break;

			case SDL_EVENT_MOUSE_BUTTON_UP: {
				System::bMouseBtnDown = false;
			} break;

			case SDL_EVENT_WINDOW_RESIZED: {
				//Graphics::bWindowResized = true;
				Graphics::WindowWidth = event.window.data1;
				Graphics::WindowHeight = event.window.data2;

			} break;

		default: break;
		}
	}

	// handle button clicks	
	UI::ProcessButtons();
}

void UpdateGame() {
	// compute time delta, store it in global variable
	static uint64_t millisecondsNow = 0;

	millisecondsNow = SDL_GetTicks();

	System::millisecondsDelta = millisecondsNow - System::millisecondsLastFrame;
	System::millisecondsLastFrame = millisecondsNow;

	// perform mouse cursor vs buttons aabb intersection test within threshold
	UI::AABBMouseVsButtons();

	if((Game::charsSelected > 0) && (UI::currentScreen->scrnType == ScreenType::BattleOn)) {

		if (Game::bNewRoundBegginig) {
			// this code should be executed every new round

			if (Game::currentRound == 1 && Game::stepsMade == 0) {
				// if this is first round initialize hero with what ever player selected
				Game::InitPlayer();
			}

			Game::ResetPlayer();
			Game::InitRandMonster();
			Game::DecideWhoStrikesFirst();
			
			Anim::attackerEntityAnimations = Anim::SetEntityAnimationByAnimType(Game::attacker, Anim::AnimType::Idle);
			Anim::victimEntityAnimations = Anim::SetEntityAnimationByAnimType(Game::victim, Anim::AnimType::Idle);
			
			Game::UpdateSpritePositions();
			UI::UpdateHealthbars();
			Anim::bIsActiveAnimationEnded = Anim::AestheticDelayCounter();

			Game::bNewRoundBegginig = false;
		}

		if (Anim::bIsActiveAnimationEnded) {

			Game::CheckRoundEndConditions();

			if(Game::stepsMade > 0)	Game::SwapAttackerAndVictim();

			// compute outcome for the next step
			Game::ComputeStepOutcome();

			// set apropriate animations
			Anim::SetAnimationsByTypes();

			// healthbar
			UI::UpdateHealthbars();

			//SetSounds();
		}
		
		Anim::AdvanceAnimations();

		// if atack animation has ended, set idle animation
		if (Anim::attackerEntityAnimations->bAnimHasEnded) {
			Anim::attackerEntityAnimations = Anim::SetEntityAnimationByAnimType(Game::attacker, Anim::AnimType::Idle);
		}

		if (Anim::attackerEntityAnimations->bIsLooping) {
			Anim::bIsActiveAnimationEnded = Anim::AestheticDelayCounter();
		}
	}
}

void RenderGame() {

	SDL_SetRenderDrawColor(Graphics::Renderer, 80, 128, 80, 255);
	SDL_RenderClear(Graphics::Renderer);
	
	// draw static ui stuff
	UI::DrawTextDescriptions();
	UI::DrawWeaponIcon();
	UI::DrawStats();

	// draw dynamic ui stuff
	UI::DrawHealthBarsAndHealth();
	UI::DrawButtons();
	UI::DrawAnimatedSprites();

	SDL_SetRenderLogicalPresentation(Graphics::Renderer, Graphics::TargetWindowWidth, 
		Graphics::TargetWindowHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	SDL_RenderPresent(Graphics::Renderer);
}

void FreeResources() {

	for (SDL_Texture* txt : UI::buttonTypes->getBtnTextures()) if(txt) SDL_DestroyTexture(txt);

	for (Screen* pScrn : *UI::screens) if(pScrn->backgroundImage) SDL_DestroyTexture(pScrn->backgroundImage);

	for (SDL_Texture* txt : UI::rndBackground->Images) if(txt) SDL_DestroyTexture(txt);

	for (TextureResourceEntry& rEntr : *Resources::ui_textures) if(rEntr.txt) SDL_DestroyTexture(rEntr.txt);
	
	TTF_CloseFont(Resources::buttonsFont);
	TTF_CloseFont(Resources::statsFont);
	TTF_CloseFont(Resources::descLargeTextFont);
	TTF_CloseFont(Resources::descSmallTextFont);
	TTF_CloseFont(Resources::gameTitleFont);
	TTF_CloseFont(Resources::descAboutTextFont);
}

void DestroyGame() {

	SDL_DestroyRenderer(Graphics::Renderer);
	SDL_DestroyWindow(Graphics::Window);
	TTF_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[]) {

	// stack allocated UI stuff, used through the game
	Screen MainMenu{};
	Screen CharacterSelection{};
	Screen BattleOn{};
	Screen VictoryUpgradeWeapon{};
	Screen VictoryUpgradeChar{};
	Screen Defeated{};
	Screen GameCompleted{};
	Screen Settings{};
	Screen About{};
	
	std::array<Screen*, 9> scrns = { 
		&MainMenu, 
		&CharacterSelection, 
		&BattleOn, 
		&VictoryUpgradeWeapon, 
		&VictoryUpgradeChar,
		&Defeated,
		&GameCompleted, 
		&Settings, 
		&About 
	};
	UI::screens = &scrns;

	ButtonType btnTypes{};
	UI::buttonTypes = &btnTypes;

	Background randomBackground{};
	UI::rndBackground = &randomBackground;
	
	// gampeplay stuff
	
	// default characters
	 const std::array<Character, 3> defCharacters = {
		Character{EntityType::Character, 2,	4,	1, 0, 0, 0, 0, 0, { 1, 4, 7 }, Character_Type::Outlaw	},	// 0
		Character{EntityType::Character, 0,	5,  1, 0, 0, 0, 0, 0, { 2, 5, 8 }, Character_Type::Warrior	},	// 1
		Character{EntityType::Character, 1,	6,	1, 0, 0, 0, 0, 0, { 3, 6, 9 }, Character_Type::Barbarian }, // 2
	 };

	Game::defaultCharacters = &defCharacters;

	Character player{};
	Game::playerCharacter = &player;

	// initialize "selected chrarcters" with defautl values
	std::array<Character_Type, 3> selectedChTypes{}; 
	selectedChTypes[0] = Character_Type::NOT_SET;
	selectedChTypes[1] = Character_Type::NOT_SET;
	selectedChTypes[2] = Character_Type::NOT_SET;

	Game::selectedCharTypes = &selectedChTypes;

	// default weapons
	 const std::array< Weapon, 6> defWpns = {
	   Weapon{ WeaponType::Sword,			DamageType::Chopping, 3, 13}, // 0
	   Weapon{ WeaponType::Club,			DamageType::Crushing, 3, 14}, // 1
	   Weapon{ WeaponType::Dagger,			DamageType::Piercing, 2, 15}, // 2
	   Weapon{ WeaponType::Axe,				DamageType::Chopping, 4, 16}, // 3
	   Weapon{ WeaponType::Spear,			DamageType::Piercing, 3, 17}, // 4
	   Weapon{ WeaponType::LegendarySword,	DamageType::Chopping, 10, 18}, // 5
	 };
	Game::defaultWeapons = &defWpns;

	Weapon currentWpn{};
	Weapon newWpn{};

	Game::currentWeapon = &currentWpn;
	Game::newWeapon		= &newWpn;
	
	// default monsters
	const std::array< Monster, 6> defMonsters = {
	  Monster{EntityType::Monster, 2,	5,	1, 0, 2, 1, 1, 1, { 0, 0, 0 }, Monster_Type::Goblin,	}, // 0
	  Monster{EntityType::Monster, 1,	10, 1, 0, 2, 2, 2, 1, { 10,0, 0 }, Monster_Type::Skeleton,	}, // 1
	  Monster{EntityType::Monster, 4,	8,	1, 0, 1, 3, 1, 2, { 11,0, 0 }, Monster_Type::Slime,		}, // 2
	  Monster{EntityType::Monster, 0,	6,	1, 0, 3, 1, 3, 1, { 1, 0, 0 }, Monster_Type::Ghost,		}, // 3
	  Monster{EntityType::Monster, 3,	10,	1, 0, 1, 3, 1, 3, { 6, 0, 0 }, Monster_Type::Golem,		}, // 4
	  Monster{EntityType::Monster, 5,	20,	1, 0, 4, 3, 3, 3, { 12,0, 0 }, Monster_Type::Dragon,	}, // 5
	};
	Game::defaultMonsters = &defMonsters;

	Monster	randomMnstr{};
	Game::randomMonster = &randomMnstr;

	std::array<LevelBonus, 13> lvlBonuses{
		LevelBonus{0,  false, false, false, nullptr		  },// 0
		LevelBonus{40, false, true,  false, &LevelBonus_40},// 1
		LevelBonus{41, false, true,  false, &LevelBonus_41},// 2
		LevelBonus{42, false, true,  false, &LevelBonus_42},// 3
		LevelBonus{43, true,  false, false, &LevelBonus_43},// 4
		LevelBonus{44, false, false, true,  &LevelBonus_44},// 5
		LevelBonus{45, false, false, true,  &LevelBonus_45},// 6
		LevelBonus{46, false, true,  false, &LevelBonus_46},// 7
		LevelBonus{47, true,  false, false, &LevelBonus_47},// 8
		LevelBonus{48, true,  false, false, &LevelBonus_48},// 9
		LevelBonus{49, false, false, true,  &LevelBonus_49},// 10
		LevelBonus{50, false, false, true,  &LevelBonus_50},// 11
		LevelBonus{51, false, true,  false, &LevelBonus_51},// 12
	};
	Game::levelBonuses = &lvlBonuses;

	// weapons and other ui, icons
	std::array<TextureResourceEntry, 3> ui_txt = {
		TextureResourceEntry{nullptr, {0,0}},
		TextureResourceEntry{nullptr, {0,0}},
		TextureResourceEntry{nullptr, {0,0}},
	};
	Resources::ui_textures = &ui_txt;

	// anim resources
	// default frame counts and image dims for different animations
	const std::array<AnimInfo, 3> defPlyrAnimInfos = {
		AnimInfo{96, {384, 384, 384, 576, 192, 384}, {4, 4, 4, 6, 2, 4} },	// 0 outlaw
		AnimInfo{96, {384, 384, 384, 480, 288, 384}, {4, 4, 4, 5, 3, 4} },	// 1 warrior
		AnimInfo{96, {384, 288, 288, 480, 192, 384}, {4, 3, 3, 5, 2, 4} },	// 2 barbarian
	};
	Anim::defPlayerAnimInfos = &defPlyrAnimInfos;
		
	const std::array<AnimInfo, 6> defMnstrAnimInfos = {
		AnimInfo{96,  {384, 384, 288, 480, 192, 384},	{4, 4, 3, 5, 2, 4} },	// 0 goblin
		AnimInfo{128, {640, 768, 512, 896, 256, 512},	{5, 6, 4, 7, 2, 4} },	// 1 skeleton
		AnimInfo{128, {512, 512, 640, 1024,768, 384},	{4, 4, 5, 8, 6, 3} },	// 2 slime
		AnimInfo{128, {512, 512, 896, 640, 384, 512},	{4, 4, 7, 5, 3, 4} },	// 3 ghost
		AnimInfo{96,  {384, 480, 192, 480, 192, 384},	{4, 5, 2, 5, 2, 4} },	// 4 golem
		AnimInfo{128, {1792, 1408, 1664, 768, 384, 640},{14, 11, 13, 6, 3, 5}},	// 5 dragon
	};
	Anim::defMonsterAnimInfos = &defMnstrAnimInfos;

	Animation playerCurrAnim{};
	playerCurrAnim.entity = Game::playerCharacter;
	Anim::playerAnimations = &playerCurrAnim;

	Animation monsterCurrAnim{};
	monsterCurrAnim.entity = Game::randomMonster;
	Anim::monsterAnimations = &monsterCurrAnim;

	std::array<TTF_Font*, 7> fnts{};
	Resources::fonts = &fnts;

	/////////////////////////////
	// function calls begin here 
	/////////////////////////////

	Game::bGameIsRunning = InitializeGraphics();

	LoadResources();

	InitializeScreens();
	InitializeLocalization();
	UpdateButtonsAndUiTextPositions();

	while (Game::bGameIsRunning) {
		ProcessPlayerInput();
		UpdateGame();
		RenderGame();
	}

	FreeResources();
	DestroyGame();

	return 0;
}

