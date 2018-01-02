#include "ui.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "dep/imgui/imgui.h"

namespace {

unsigned int screenWidth = 500;
unsigned int screenHeight = 500;

bool keyboardEvent(unsigned char nChar, int nX, int nY) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(nChar);

	if ( nChar == 27 ) { // Esc-key
		glutLeaveMainLoop();
	}
	return true;
}

void keyboardSpecial(int key, int x, int y) {
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(key);
}

bool mouseEvent(int button, int state, int x, int y) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);

	if (state == GLUT_DOWN && (button == GLUT_LEFT_BUTTON)) {
		io.MouseDown[0] = true;
	} else {
		io.MouseDown[0] = false;
	}
	if (state == GLUT_DOWN && (button == GLUT_RIGHT_BUTTON)) {
		io.MouseDown[1] = true;
	} else {
		io.MouseDown[1] = false;
	}

	if ( (button == 3) || (button == 4) ) { // wheel
		if ( button == 3 ) {
			io.MouseWheel = 1.0;
		} else {
			io.MouseWheel = -1.0;
		}
	}

	return true;
}

void keyboardCallback(unsigned char nChar, int x, int y) {
	if (keyboardEvent(nChar, x, y)) {
		glutPostRedisplay();
	}
}

void mouseCallback(int button, int state, int x, int y) {
	if (mouseEvent(button, state, x, y)) {
		glutPostRedisplay();
	}
}

void mouseDragCallback(int x, int y) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	glutPostRedisplay();
}

void mouseMoveCallback(int x, int y) {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
	glutPostRedisplay();
}

void reshape(int w, int h) {
	screenWidth = w;
	screenHeight = h;
	glViewport(0, 0, w, h);
}

}

namespace ui {

void installCallbacks() {
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(keyboardSpecial);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseDragCallback);
	glutPassiveMotionFunc(mouseMoveCallback);
}

unsigned int getScreenWidth() {
	return screenWidth;
}

unsigned int getScreenHeight() {
	return screenHeight;
}

}
