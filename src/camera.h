#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

class Camera {
private:
	float rotation_z_ = 0.0;
	float rotation_x_ = 0.0;
	glm::vec3 up_       = glm::vec3(0.0,  0.0, 1.0);
	glm::vec3 origin_   = glm::vec3(0.0,  0.0, 0.0);
	glm::vec3 position_ = glm::vec3(0.0, -5.0, 5.0);

	GLint uniform_transform_;

public:
	void setup(GLint shader);
	void apply(int screenWidth, int screenHeight);

	void drawUI();

};
