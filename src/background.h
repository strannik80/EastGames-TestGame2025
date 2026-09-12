#pragma once

#include <SDL3_image/SDL_image.h>
#include <vector>
#include "utility.h"


struct Background {
private:
	uint8_t lastRandomIdx = 0;
public:
	std::vector<SDL_Texture*> Images;

	SDL_Texture* GetRandomBackground() {
		uint8_t idx = 0;
	
		idx = SDL_rand( static_cast<Sint32>(Images.size()) );
		if (idx == lastRandomIdx) {
			if( (idx + 1) == Images.size() ) --idx;
			else ++idx;
		}
	
		lastRandomIdx = idx;
	
		return	Images[idx];
	}
};
