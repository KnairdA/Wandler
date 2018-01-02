#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "dep/imgui/imgui.h"
#include "dep/imgui_glut/imgui_impl_glut.h"

#include <vector>

#include "ui.h"
#include "util.h"
#include "camera.h"

Camera camera;

bool drawXPlane = true;
bool drawYPlane = true;
bool drawZPlane = true;
bool drawAxis   = true;

const float grid = 0.2;

std::vector<GLfloat> vertices;
GLuint vertex_buffer;

glm::mat3 field_transform = glm::mat3(
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0
);
float alpha = 1.0;

GLint uniform_field_transform;
GLint uniform_alpha;
GLint shader;

int extend = 5;

void reset() {
	vertices.clear();
	vertices.reserve(18*(2*extend+1)*(2*extend+1));

	int idxZ = 0;
	int idxX = 2*(2*extend+1)*(2*extend+1);
	int idxY = 4*(2*extend+1)*(2*extend+1);

	for ( int i = 0; i < 20*(2*extend+1)*(2*extend+1); ++i ) {
		vertices.emplace_back();
	}

	for ( int i = -extend; i <= extend; ++i ) {
		for ( int j = -extend; j <= extend; ++j ) {
			vertices[3*idxZ+0] = i*grid;
			vertices[3*idxZ+1] = i % 2 == 0 ? j*grid : (-j)*grid;
			vertices[3*idxZ+2] = 0.0;

			vertices[3*idxX+0] = 0.0;
			vertices[3*idxX+1] = i*grid;
			vertices[3*idxX+2] = i % 2 == 0 ? j*grid : (-j)*grid;

			vertices[3*idxY+0] = i*grid;
			vertices[3*idxY+1] = 0.0;
			vertices[3*idxY+2] = i % 2 == 0 ? j*grid : (-j)*grid;

			idxZ++; idxY++; idxX++;
		}
	}

	for ( int j = -extend; j <= extend; ++j ) {
		for ( int i = -extend; i <= extend; ++i ) {
			vertices[3*idxZ+0] = j % 2 == 0 ? (-i)*grid : i*grid;
			vertices[3*idxZ+1] = j*grid;
			vertices[3*idxZ+2] = 0.0;

			vertices[3*idxX+0] = 0.0;
			vertices[3*idxX+1] = j % 2 == 0 ? (-i)*grid : i*grid;
			vertices[3*idxX+2] = j*grid;

			vertices[3*idxY+0] = j % 2 == 0 ? (-i)*grid : i*grid;
			vertices[3*idxY+1] = 0.0;
			vertices[3*idxY+2] = j*grid;

			idxZ++; idxY++; idxX++;
		}
	}
}

void drawUI() {
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	ImGui_ImplGLUT_NewFrame(ui::getScreenWidth(), ui::getScreenHeight());

	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("View");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Spacing();
	if ( ImGui::SliderInt("Grid extend", &extend, 1, 20) ) {
		reset();
	}
	ImGui::Spacing();
	ImGui::Checkbox("X plane", &drawXPlane);
	ImGui::SameLine();
	ImGui::Checkbox("Y plane", &drawYPlane);
	ImGui::SameLine();
	ImGui::Checkbox("Z plane", &drawZPlane);
	ImGui::SameLine();
	ImGui::Checkbox("Axis",    &drawAxis);
	ImGui::Spacing();
	if ( ImGui::Button("Reset camera") ) {
		camera.reset();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Lock X", &camera.xAxisIsLocked);
	ImGui::SameLine();
	ImGui::Checkbox("Lock Z", &camera.zAxisIsLocked);
	ImGui::Spacing();
	ImGui::SliderFloat("Distance", &camera.distance, 0.1f, 20.f);
	ImGui::End();

	ImGui::Begin("Transformation");
	ImGui::DragFloat3("row1", (float*)&(field_transform[0]), 0.1f, -2.0f, 2.0f);
	ImGui::DragFloat3("row2", (float*)&(field_transform[1]), 0.1f, -2.0f, 2.0f);
	ImGui::DragFloat3("row3", (float*)&(field_transform[2]), 0.1f, -2.0f, 2.0f);
	ImGui::Text("Determinant: %.3f", glm::determinant(field_transform));
	ImGui::Spacing();
	ImGui::SliderFloat("alpha", &alpha, 0.0, 1.0);
	ImGui::End();

	ImGui::Render();
	glPopAttrib();
	glPopMatrix();

	camera.processUserInput();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	drawUI();

	glUseProgram(shader);
	camera.publishUniform(ui::getScreenWidth(), ui::getScreenHeight());
	glUniformMatrix3fv(uniform_field_transform,  1, GL_FALSE, glm::value_ptr(field_transform));
	glUniform1f(uniform_alpha, alpha);

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glLineWidth(1.0);
	glEnable(GL_LINE_SMOOTH);
	if ( drawZPlane ) {
		glColor3f(1.0,0.0,0.0);
		glDrawArrays(GL_LINE_STRIP, 0,                             2*(2*extend+1)*(2*extend+1)-0);
	}
	if ( drawXPlane ) {
		glColor3f(0.0,1.0,0.0);
		glDrawArrays(GL_LINE_STRIP, 2*(2*extend+1)*(2*extend+1)+1, 2*(2*extend+1)*(2*extend+1)-1);
	}
	if ( drawYPlane ) {
		glColor3f(0.0,0.0,1.0);
		glDrawArrays(GL_LINE_STRIP, 4*(2*extend+1)*(2*extend+1)+1, 2*(2*extend+1)*(2*extend+1)-1);
	}
	glDeleteBuffers(1, &vertex_buffer);
	glDisableVertexAttribArray(0);

	if ( drawAxis ) {
		glLineWidth(4.0);
		glColor3f(1.0,1.0,1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(extend*grid,0.0,0.0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,extend*grid,0.0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,0.0,extend*grid);
		glEnd();
	}

	glUseProgram(0);

	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	reset();

	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutCreateWindow("Wandler");
	glewInit();

	shader = glCreateProgram();
	glAttachShader(shader, util::compileShader(
		"uniform mat4 camera_transform;"
		"uniform mat3 field_transform;"
		"uniform float alpha;"
		"void main(void) {"
			"mat3 partial_field_transformation = (mat3(1.0) + alpha * (field_transform - mat3(1.0)));"
			"vec3 vertex = vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z);"
			"gl_Position = camera_transform"
			"            * vec4(partial_field_transformation * vertex, 1.0);"
			"gl_FrontColor = gl_Color;"
		"}\n",
		GL_VERTEX_SHADER));
	glAttachShader(shader, util::compileShader(
		"void main(void) { gl_FragColor = gl_Color; }\n",
		GL_FRAGMENT_SHADER));
	glLinkProgram(shader);
	GLint linked;
	glGetShaderiv(shader, GL_LINK_STATUS, &linked);

	camera.setup(shader);

	uniform_field_transform  = util::getUniform(shader, "field_transform");
	uniform_alpha = util::getUniform(shader, "alpha");

	ImGui_ImplGLUT_Init();

	ui::installCallbacks();

	glutDisplayFunc(display);

	glutMainLoop();

	ImGui_ImplGLUT_Shutdown();
}
