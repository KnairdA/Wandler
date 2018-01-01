#include "camera.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "dep/imgui/imgui.h"

void Camera::setup(GLint shader) {
	uniform_transform_ = glGetUniformLocation(shader, "camera_transform");
}

void Camera::reset() {
	model_ = glm::quat();
	drag_model_ = glm::quat();
}

void Camera::apply(int screenWidth, int screenHeight) {
	glm::mat4 view = glm::lookAt(glm::vec3(0.f,-distance_,0.f), origin_, up_);
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * screenWidth / screenHeight, 0.0f, 1.0f);
	glm::mat4 model;

	if ( ImGui::IsMouseDragging() && !ImGui::GetIO().WantCaptureMouse ) {
		ImVec2 drag = ImGui::GetMouseDragDelta();
		drag_model_ = model_;
		glm::quat conjugate_drag_model = glm::conjugate(drag_model_);
		drag_model_ *= glm::angleAxis(drag.y * 0.01f, conjugate_drag_model*glm::vec3(1.f,0.f,0.f));
		drag_model_ *= glm::angleAxis(drag.x * 0.01f, conjugate_drag_model*glm::vec3(0.f,0.f,1.f));

		model = glm::toMat4(drag_model_);
	} else {
		if ( model_ != drag_model_ ) {
			model_ = drag_model_;
		}

		model = glm::toMat4(model_);
	}

	glUniformMatrix4fv(uniform_transform_, 1, GL_FALSE, glm::value_ptr(projection * view * model));
}
