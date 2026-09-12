#pragma once

#include <SDL3/SDL.h>
#include <array>
#include "utility.h"
#include "animation.h"
#include "texture_resource_entry.h"

using Utility::vec2;

struct Sprite {
	Animation* anim = nullptr;
	TextureResourceEntry* txtRs = nullptr;
	bool bIsSpriteAnimated = true;
	vec2<uint16_t> pos{};
};