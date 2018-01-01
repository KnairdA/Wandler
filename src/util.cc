#include "util.h"

#include <vector>
#include <iostream>

namespace util {

GLint getUniform(GLuint program, const std::string& name) {
	const GLint uniform = glGetUniformLocation(program, name.c_str());
	if ( uniform == -1 ) {
		std::cerr << "Could not bind uniform " << name << std::endl;
	}
	return uniform;
}

GLint compileShader(const std::string& source, GLenum type) {
	GLint local_shader = glCreateShader(type);

	if ( !local_shader ) {
		std::cerr << "Cannot create a shader of type " << type << std::endl;
		exit(-1);
	}

	const char* source_data = source.c_str();
	const int source_length = source.size();

	glShaderSource(local_shader, 1, &source_data, &source_length);
	glCompileShader(local_shader);

	GLint compiled;
	glGetShaderiv(local_shader, GL_COMPILE_STATUS, &compiled);
	if ( !compiled ) {
		std::cerr << "Cannot compile shader" << std::endl;
		GLint maxLength = 0;
		glGetShaderiv(local_shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(local_shader, maxLength, &maxLength, &errorLog[0]);
		for( auto c : errorLog ) {
			std::cerr << c;
		}
		std::cerr << std::endl;
	}

	return local_shader;
}

}
