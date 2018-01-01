#pragma once

#include <GL/glew.h>

#include <string>

namespace util {

GLint getUniform(GLuint program, const std::string& name);
GLint compileShader(const std::string& source, GLenum type);

}
