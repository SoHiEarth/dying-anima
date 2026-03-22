#include "game/degradation.h"

glm::vec3 game::DegradeColor(const glm::vec3& color, DegradationLevel level) {
	switch (level) {
		case LEVEL_0:
			return color;
		case LEVEL_1:
			return color * 0.75f;
		case LEVEL_2:
			return color * 0.5f;
		case LEVEL_3:
			return color * 0.25f;
		default:
			return color;
	}
}