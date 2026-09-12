#pragma once
#include <SDL3/SDL.h>
#include "utility.h"

struct TextureResourceEntry {
	SDL_Texture* txt = nullptr;
	Utility::vec2<int16_t> dims{0, 0};
};
