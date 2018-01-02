#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera {
private:
	float rotation_z_ = 0.0;
	float rotation_x_ = 0.0;

	glm::vec3 up_     = glm::vec3(0.0,  0.0, 1.0);
	glm::vec3 origin_ = glm::vec3(0.0,  0.0, 0.0);

	glm::quat* model_;

	glm::quat rest_model_;
	glm::quat drag_model_;

	GLint uniform_transform_;

public:
	float distance      = 5.0;
	bool  xAxisIsLocked = false;
	bool  zAxisIsLocked = false;

	void setup(GLint shader);
	void reset();

	void processUserInput();

	void publishUniform(int screenWidth, int screenHeight);

};
