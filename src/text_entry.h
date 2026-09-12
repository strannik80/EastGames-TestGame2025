#pragma once

#include <stdint.h>
#include "utility.h"
#include "entity.h"

struct TextEntry {
	Utility::vec2<uint16_t> pos{ 0, 0 };
	uint8_t textIdx = 0;
	uint8_t fontIdx = 0;
	bool bIsVisible = true;
	//bool bIsDynamic = false;
	uint8_t(*fnToInvoke)() = nullptr;
};