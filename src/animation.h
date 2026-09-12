#pragma once

#include <SDL3/SDL.h>
#include <array>
#include "entity.h"

namespace System
{
	extern uint64_t millisecondsDelta;
}

namespace Anim
{
	extern uint64_t strikeAnimFrameThreshold;
	extern uint64_t idleAnimFrameThreshold;
	extern uint64_t idleHurtAnimFrameThreshold;
	extern uint64_t deathAnimFrameThreshold;
}

struct AnimInfo {
	const uint16_t images_h = 0;
	const std::array<uint16_t, 6>	imageWidths;
	const std::array<uint8_t, 6>	frameCounts;
}; 

struct Animation {
	std::array<SDL_Texture*, 6> animTextures;
	Entity* entity = nullptr;
	uint64_t currFrameMs = 0;
	//uint64_t animDurationMs = 0;
	uint8_t currentAnimTxtIdx = 0;
	uint8_t frameCount = 0;
	uint8_t framesElapsed = 0;
	uint8_t currentFrame = 0;
	bool bAnimHasEnded = false;
	bool bIsLooping = true;

	void AdvanceAnimation() {
		if (entity->health == 0 && currentFrame == (frameCount - 1)) return;

		currFrameMs += System::millisecondsDelta;
		uint64_t threshold = 0;

		if (currentAnimTxtIdx < 3) threshold = Anim::strikeAnimFrameThreshold;
		else if(currentAnimTxtIdx == 5) threshold = Anim::deathAnimFrameThreshold;
		else if (currentAnimTxtIdx == 3) threshold = Anim::idleAnimFrameThreshold;
		else threshold = Anim::idleHurtAnimFrameThreshold;

		if (currFrameMs >= threshold) {
			currFrameMs -= threshold;

			++currentFrame;
			currentFrame %= frameCount;

			if (!bIsLooping) {
				++framesElapsed;
				if (framesElapsed >= frameCount) {
					bAnimHasEnded = true;
					framesElapsed = 0;
					currentFrame = 0;
				}
			}
		}			
	}
};
