#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tuple>
#include <cmath>
#include <vector>
#include <thread>

#include "dep/imgui/imgui.h"
#include "dep/imgui_glut/imgui_impl_glut.h"

#include "ui.h"

float rotation = 0.0;
float alpha    = 1.0;
float grid     = 0.2;

bool drawXPlane = true;
bool drawYPlane = true;
bool drawZPlane = true;
bool drawAxis   = true;

GLfloat vertices[18*20*20];
GLuint vertex_buffer;

glm::mat3 field_transform = glm::mat3(
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0
);

GLint uniform_field_transform;
GLint uniform_camera_transform;
GLint uniform_alpha;

const GLint* shader;

void reset() {
	int idx = 0;
	for ( int i = -10; i < 10; i += 1 ) {
		for ( int j = -10; j < 10; j += 1 ) {
			vertices[3*idx+0] = i*grid;
			vertices[3*idx+1] = i % 2 == 0 ? j*grid : (-j-1)*grid;
			vertices[3*idx+2] = 0.0;
			idx++;
		}
	}
	for ( int j = -10; j < 10; j += 1 ) {
		for ( int i = -10; i < 10; i += 1 ) {
			vertices[3*idx+0] = j % 2 == 0 ? (-i-1)*grid : i*grid;
			vertices[3*idx+1] = j*grid;
			vertices[3*idx+2] = 0.0;
			idx++;
		}
	}

	for ( int i = -10; i < 10; i += 1 ) {
		for ( int j = -10; j < 10; j += 1 ) {
			vertices[3*idx+0] = i*grid;
			vertices[3*idx+1] = 0.0;
			vertices[3*idx+2] = i % 2 == 0 ? j*grid : (-j-1)*grid;
			idx++;
		}
	}
	for ( int j = -10; j < 10; j += 1 ) {
		for ( int i = -10; i < 10; i += 1 ) {
			vertices[3*idx+0] = j % 2 == 0 ? (-i-1)*grid : i*grid;
			vertices[3*idx+1] = 0.0;
			vertices[3*idx+2] = j*grid;
			idx++;
		}
	}

	for ( int i = -10; i < 10; i += 1 ) {
		for ( int j = -10; j < 10; j += 1 ) {
			vertices[3*idx+0] = 0.0;
			vertices[3*idx+1] = i*grid;
			vertices[3*idx+2] = i % 2 == 0 ? j*grid : (-j-1)*grid;
			idx++;
		}
	}
	for ( int j = -10; j < 10; j += 1 ) {
		for ( int i = -10; i < 10; i += 1 ) {
			vertices[3*idx+0] = 0.0;
			vertices[3*idx+1] = j % 2 == 0 ? (-i-1)*grid : i*grid;
			vertices[3*idx+2] = j*grid;
			idx++;
		}
	}
}

void drawUI() {
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	ImGui_ImplGLUT_NewFrame(screenWidth, screenHeight);

	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("View");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("rotation", &rotation, 0.0, 2*M_PI);
	if ( ImGui::SliderFloat("grid", &grid, 0.01, 1.0) ) {
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
	ImGui::End();

	ImGui::Begin("Transformation");
	ImGui::SliderFloat3("row1", (float*)&(field_transform[0]), -2.0f, 2.0f);
	ImGui::SliderFloat3("row2", (float*)&(field_transform[1]), -2.0f, 2.0f);
	ImGui::SliderFloat3("row3", (float*)&(field_transform[2]), -2.0f, 2.0f);
	ImGui::Spacing();
	ImGui::SliderFloat("alpha", &alpha, 0.0, 1.0);
	ImGui::End();

	ImGui::Render();
	glPopAttrib();
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	drawUI();

	glUseProgram(*shader);

	glm::mat4 model = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, -5.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * 640 / 480, 0.0f, 10.0f);
	glm::mat4 camera_transform = projection * view * model;

	glUniformMatrix4fv(uniform_camera_transform, 1, GL_FALSE, glm::value_ptr(camera_transform));
	glUniformMatrix3fv(uniform_field_transform,  1, GL_FALSE, glm::value_ptr(field_transform));
	glUniform1f(uniform_alpha, alpha);

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glLineWidth(1.0);
	glEnable(GL_LINE_SMOOTH);
	if ( drawZPlane ) {
		glColor3f(1.0,0.0,0.0);
		glDrawArrays(GL_LINE_STRIP, 0,         2*20*20);
	}
	if ( drawYPlane ) {
		glColor3f(0.0,1.0,0.0);
		glDrawArrays(GL_LINE_STRIP, 2*20*20+1, 2*20*20-1);
	}
	if ( drawXPlane ) {
		glColor3f(0.0,0.0,1.0);
		glDrawArrays(GL_LINE_STRIP, 4*20*20+1, 2*20*20-1);
	}
	glDeleteBuffers(1, &vertex_buffer);
	glDisableVertexAttribArray(0);

	if ( drawAxis ) {
		glLineWidth(4.0);
		glColor3f(1.0,1.0,1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(10*grid,0.0,0.0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,10*grid,0.0);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,0.0,10*grid);
		glEnd();
	}

	glUseProgram(0);

	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h) {
	screenWidth = w;
	screenHeight = h;
	glViewport(0, 0, w, h);
}

GLint compileShader(const std::string& source, GLenum type) {
	GLint shader = glCreateShader(type);

	if ( !shader ) {
		std::cerr << "Cannot create a shader of type " << shader << std::endl;
		exit(-1);
	}

	const char* source_data = source.c_str();
	const int source_length = source.size();

	glShaderSource(shader, 1, &source_data, &source_length);
	glCompileShader(shader);

	{
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if ( !compiled ) {
			std::cerr << "Cannot compile shader" << std::endl;

			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
			for( auto c : errorLog ) {
				std::cerr << c;
			}
			std::cerr << std::endl;

			exit(-1);
		}
	}

	return shader;
}

GLint get_uniform(GLuint program, const char *name) {
	GLint uniform = glGetUniformLocation(program, name);
	if ( uniform == -1 ) {
		std::cerr << "Could not bind uniform " << name << std::endl;
	}
	return uniform;
}

int main(int argc, char** argv) {
	reset();

	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutCreateWindow("Wandler");
	glewInit();

	GLint local_shader = glCreateProgram();
	glAttachShader(local_shader, compileShader(
		"uniform mat4 camera_transform;"
		"uniform mat3 field_transform;"
		"uniform float alpha;"
		"void main(void) {"
			"gl_Position = camera_transform * vec4((mat3(1.0) + alpha * (field_transform - mat3(1.0))) * vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z), 1.0);"
			//"gl_Position = camera_transform * vec4(sin(gl_Vertex.x), sin(gl_Vertex.y), sin(gl_Vertex.z), 1.0);"
			"gl_FrontColor = gl_Color;"
		"}\n",
		GL_VERTEX_SHADER));
	glAttachShader(local_shader, compileShader(
		"void main(void) { gl_FragColor = gl_Color; }\n",
		GL_FRAGMENT_SHADER));
	glLinkProgram(local_shader);
	GLint linked;
	glGetShaderiv(local_shader, GL_LINK_STATUS, &linked);
	uniform_camera_transform = get_uniform(local_shader, "camera_transform");
	uniform_field_transform  = get_uniform(local_shader, "field_transform");
	uniform_alpha = get_uniform(local_shader, "alpha");
	shader = &local_shader;

	ImGui_ImplGLUT_Init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(KeyboardSpecial);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseDragCallback);
	glutPassiveMotionFunc(mouseMoveCallback);

	glutMainLoop();

	ImGui_ImplGLUT_Shutdown();
}
