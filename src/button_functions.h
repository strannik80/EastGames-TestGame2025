#pragma once

#include "screen.h"
#include "entity.h"

namespace Game
{
	extern bool bGameIsRunning;
	extern bool bNewRoundBegginig;

	extern const uint8_t maxCharLevel;
	
	//extern uint8_t victoryCount;
	extern uint8_t currentRound;

	extern uint8_t charsSelected;
	extern Character_Type newlySelecterCharType;
	 
	extern std::array<Character_Type, 3>* selectedCharTypes;
		
	extern Character* playerCharacter;

	extern Weapon* currentWeapon;
	extern Weapon* newWeapon;


	extern void ResetGame();
}

namespace UI
{
	extern Screen* currentScreen;
	extern std::array<Screen*, 9>* screens;
	extern Background* rndBackground;

	extern std::array<const std::string, 30>* btnText;
	extern std::array<const std::string, 60>* descText;
}

namespace Localization
{
	extern std::array<const std::string, 30> btnTxtEnglish;
	extern std::array<const std::string, 60> descTxtEnglish;

	extern std::array<const std::string, 30> btnTxtRussian;
	extern std::array<const std::string, 60> descTxtRussian;
}



namespace BtnFuncitons 
{
	uint8_t aestheticDelay = 0;

	void SetRandomBackground() {
		UI::currentScreen->backgroundImage = UI::rndBackground->GetRandomBackground();
	}

	void GoTo_CharacterSelection_Screen() {
		SDL_Delay(aestheticDelay);

		// if any characters already selected, set corresponding buttons inactive
		Screen* CharacterSelectionScreen = (*UI::screens)[1];
		uint8_t idx = 0;
		for (Character_Type chType : (*Game::selectedCharTypes)){
			if (chType != Character_Type::NOT_SET) {
				idx = static_cast<uint8_t>(chType);

				CharacterSelectionScreen->getButtons()[idx].bIsToggledOn = false;
				CharacterSelectionScreen->getButtons()[idx].bIsActive = false;
			}
		}

		(CharacterSelectionScreen->getButtons())[3].bIsActive = false;

		UI::currentScreen = (*UI::screens)[1];
	}

	void GoTo_Settings_Screen() {

		SDL_Delay(aestheticDelay);

		UI::currentScreen = (*UI::screens)[7];
	}
		
	void EquipNewWeapon_And_GoTo_CharacterUpgrade_Screen() {
		SDL_Delay(aestheticDelay);

		Game::playerCharacter->wpnIdx = static_cast<uint8_t>(Game::newWeapon->wpnType);
		*Game::currentWeapon = *Game::newWeapon;

		if ((Game::charsSelected < Game::maxCharLevel)
			&& (Game::playerCharacter->charLevel < Game::maxCharLevel)){
			UI::currentScreen = (*UI::screens)[4];
		}
		else {
			UI::currentScreen = (*UI::screens)[2];
		}
	}
	
	void DismissNewWeapon_And_GoTo_CharacterUpgrade_Screen() {
		SDL_Delay(aestheticDelay);

		if ((Game::charsSelected < Game::maxCharLevel) 
			&& (Game::playerCharacter->charLevel < Game::maxCharLevel)){

			UI::currentScreen = (*UI::screens)[4];
		}
		else {
			UI::currentScreen = (*UI::screens)[2];
		}
	}

	void ContinueWithoutMulticl_And_GoTo_BattleOn_Screen() {

		SDL_Delay(aestheticDelay);
		Game::playerCharacter->charLevel += 1;

		UI::currentScreen = (*UI::screens)[2];
		SetRandomBackground();
	}
	
	void GoTo_MainMenu_Screen() {
		SDL_Delay(aestheticDelay);

		if (UI::currentScreen->scrnType == ScreenType::Settings ||
			UI::currentScreen->scrnType == ScreenType::About ) {

			UI::currentScreen = (*UI::screens)[0];
			return;
		}

		//if(UI::currentScreen->scrnType == ScreenType::VictoryUpgradeWeapon
		//		|| UI::currentScreen->scrnType	== ScreenType::VictorySelectChar 
		//		|| (UI::currentScreen->scrnType	== ScreenType::CharacterSelection &&
		//			Game::currentRound != 0) ) {
		// 
			//	typedef struct SDL_MessageBoxData
			//	{
			//		SDL_MessageBoxFlags flags;
			//		SDL_Window* window;                 /**< Parent window, can be NULL */
			//		const char* title;                  /**< UTF-8 title */
			//		const char* message;                /**< UTF-8 message text */
			//	
			//		int numbuttons;
			//		const SDL_MessageBoxButtonData* buttons;
			//	
			//		const SDL_MessageBoxColorScheme* colorScheme;   /**< SDL_MessageBoxColorScheme, can be NULL to use system settings */
			//	} SDL_MessageBoxData;
			
			//	typedef struct SDL_MessageBoxButtonData
			//	{
			//		SDL_MessageBoxButtonFlags flags;
			//		int buttonID;       /**< User defined button id (value returned via SDL_ShowMessageBox) */
			//		const char* text;   /**< The UTF-8 button text */
			//	} SDL_MessageBoxButtonData;
			
			
			/*
			SDL_MessageBoxButtonFlags btnFlags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			const SDL_MessageBoxButtonData btns = { btnFlags, 1, "XXX"};

			SDL_MessageBoxFlags boxFlags = SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT | SDL_MESSAGEBOX_INFORMATION;
			const SDL_MessageBoxData mbd{ boxFlags, 0, "Message box title", "Message box message", 1, &btns, 0};
			int btnId = 0;
			bool msgBox = SDL_ShowMessageBox(&mbd, &btnId);

			if(btnId) SDL_Log("btnId: %d\n", btnId);
			*/

		Game::ResetGame();

		UI::currentScreen = (*UI::screens)[0];
	}
	
	void ExitGame() {

		SDL_Delay(aestheticDelay);

		Game::bGameIsRunning = false;
	}

	void ConfirmCharSelection_And_GoTo_BattleOn_Screen() {

		SDL_Delay(aestheticDelay);

		assert(Game::newlySelecterCharType != Character_Type::NOT_SET);
		assert(Game::charsSelected <= Game::maxCharLevel);

		(*Game::selectedCharTypes)[Game::charsSelected++] = Game::newlySelecterCharType;

		if (Game::currentRound != 1) Game::playerCharacter->charLevel += 1;

		UI::currentScreen = (*UI::screens)[2];
		SetRandomBackground();
	}

	void SelectCharacter_Outlaw(){

		Game::newlySelecterCharType = Character_Type::Outlaw;

		UI::currentScreen->getButtons()[0].bIsToggledOn = true;
		UI::currentScreen->getButtons()[1].bIsToggledOn = false;
		UI::currentScreen->getButtons()[2].bIsToggledOn = false;
		
		UI::currentScreen->getButtons()[3].bIsActive = true;
	}

	void SelectCharacter_Warrior() {
		
		Game::newlySelecterCharType = Character_Type::Warrior;

		UI::currentScreen->getButtons()[1].bIsToggledOn = true;
		UI::currentScreen->getButtons()[0].bIsToggledOn = false;
		UI::currentScreen->getButtons()[2].bIsToggledOn = false;

		UI::currentScreen->getButtons()[3].bIsActive = true;
	}

	void SelectCharacter_Barbarian() {

		Game::newlySelecterCharType = Character_Type::Barbarian;

		UI::currentScreen->getButtons()[2].bIsToggledOn = true;
		UI::currentScreen->getButtons()[0].bIsToggledOn = false;
		UI::currentScreen->getButtons()[1].bIsToggledOn = false;

		UI::currentScreen->getButtons()[3].bIsActive = true;
	}
	
	void GoTo_About_Screen() {
		SDL_Delay(aestheticDelay);

		UI::currentScreen = (*UI::screens)[8];
	}

	void Dbg_GoTo_VictoryWpnUpgrade_Screen() {

		UI::currentScreen = (*UI::screens)[3];
	}

	void Dbg_GoTo_Defeated_Screen() {

		UI::currentScreen = (*UI::screens)[5];
	}

	void Dbg_GoTo_Completed_Screen() {

		UI::currentScreen = (*UI::screens)[6];
	}

	void SetLocalizationEn() {

		UI::currentScreen->getButtons()[1].bIsToggledOn = false;
		UI::currentScreen->getButtons()[2].bIsToggledOn = true;

		UI::btnText = &Localization::btnTxtEnglish;
		UI::descText = &Localization::descTxtEnglish;
	}

	void SetLocalizationRus() {
		UI::currentScreen->getButtons()[2].bIsToggledOn = false;
		UI::currentScreen->getButtons()[1].bIsToggledOn = true;

		UI::btnText = &Localization::btnTxtRussian;
		UI::descText = &Localization::descTxtRussian;
	}
}