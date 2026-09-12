#pragma once

#include <SDL3_image/SDL_image.h>
#include <vector>

#include "button.h"
#include "text_entry.h"
#include "sprite.h"

enum class ScreenType : uint8_t {
	UNSET = 0,
	MainMenu,
	Settings,
	CharacterSelection,
	BattleOn,
	VictoryUpgradeWeapon,
	VictorySelectChar,
	Defeated,
	GameCompleted,
	About
};

struct Screen {
private:
	std::vector<Button> buttons;
	std::vector<TextEntry> textEntries;
	std::vector<Sprite> spriteEntries;

public:
	ScreenType scrnType				= ScreenType::UNSET;
	SDL_Texture* backgroundImage	= nullptr;

	uint8_t buttonCount	= 0;
	uint8_t textEntriesCount = 0;
	uint8_t spriteEntriesCount = 0;

	void push_back_button(Button bt) {
		buttons.push_back(std::move(bt));
		buttonCount = static_cast<uint8_t>(buttons.size());
	}

	void push_back_text_entry(TextEntry txt) {
		textEntries.push_back(std::move(txt));
		textEntriesCount = static_cast<uint8_t>(textEntries.size());
	}

	void push_back_sprite(Sprite sp) {
		spriteEntries.push_back(std::move(sp));
		spriteEntriesCount = static_cast<uint8_t>(spriteEntries.size());
	}

	//Button& operator[](uint8_t idx) { return buttons[idx]; }

	std::vector<Button>&	getButtons()		{ return buttons; }
	std::vector<TextEntry>& getTextEntries()	{ return textEntries; }
	std::vector<Sprite>&	getSpriteEntries()	{ return spriteEntries; }
};