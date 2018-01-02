#pragma once

#include <glm/glm.hpp>

#include <string>

namespace ui {

void installCallbacks();

unsigned int getScreenWidth();
unsigned int getScreenHeight();

bool FloatMatrixDrag3x3(glm::mat3* const matrix, const std::string& name);

}
