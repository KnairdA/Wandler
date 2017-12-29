#include "camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "dep/imgui/imgui.h"

void Camera::setup(GLint shader) {
	uniform_transform_ = glGetUniformLocation(shader, "camera_transform");
}

void Camera::apply(int screenWidth, int screenHeight) {
	glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 xAxis = glm::rotate(glm::vec3(1.0f, 0.0f, 0.0f), -rotation_z_, zAxis);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), rotation_z_, zAxis)
					* glm::rotate(glm::mat4(1.0f), rotation_x_, xAxis);
	glm::mat4 view  = glm::lookAt(position_, origin_, up_);
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * screenWidth / screenHeight, 0.0f, 10.0f);

	glUniformMatrix4fv(uniform_transform_, 1, GL_FALSE, glm::value_ptr(projection * view * model));
}

void Camera::drawUI() {
	ImGui::SliderFloat("rotationZ", &rotation_z_, -M_PI, M_PI);
	ImGui::SliderFloat("rotationX", &rotation_x_, -M_PI, M_PI);
}
