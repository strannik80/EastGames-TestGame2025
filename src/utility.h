#pragma once

namespace Utility {

	template<typename T>
	struct vec2 {
		T x;
		T y;
	};

	uint8_t GetRandomNumber(uint8_t max) {
		assert(max < 255);
		return static_cast<uint8_t>(SDL_rand(max)) + 1;
	}
}