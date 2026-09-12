#pragma once

#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>

#include "utility.h"


struct Button {

	Utility::vec2<float> pos{ 0,0 };
	uint8_t btnTextId = 0;

	bool bIsTogglable = false;
	bool bIsToggledOn = false;

	bool bIsActive = true;

	bool bIsHoveredOver = false;
	uint8_t btnTypeIdx = 0;

	void (*fnToInvoke)() = nullptr;
};

struct ButtonType
{
private:
	std::vector<SDL_Texture*> btnTextures;
	std::vector<Utility::vec2<uint16_t> > btnDims;
public:
	uint8_t btnTypeCount = 3;

	void push_back_entry(SDL_Texture* txt, uint16_t w, uint16_t h) {
		btnTextures.push_back(txt);
		btnDims.push_back(Utility::vec2<uint16_t>{ w, h});
	}

	SDL_Texture* getBtnTextureByTypeIdx(uint8_t tyIdx) const {

		if (tyIdx < btnTypeCount) { return btnTextures[tyIdx]; }
		else {
			spdlog::error("Button type ID can not be larger than %d.\n", btnTypeCount - 1);
			return nullptr;
		}
	}

	const Utility::vec2<uint16_t>& getBtnDimsByTypeIdx(uint8_t tyIdx) const{

		if (tyIdx < btnTypeCount) { return btnDims[tyIdx]; }
		else {
			spdlog::error("Button type ID can not be larger than %d.\n", btnTypeCount - 1);
			return btnDims[0];
		}
	}

	std::vector<SDL_Texture*>& getBtnTextures() { return btnTextures; }
};