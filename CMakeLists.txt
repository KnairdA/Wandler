cmake_minimum_required(VERSION 3.10)
project(wandler)

set(
	CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -std=c++14 -W -Wall -Wextra -Winline -pedantic"
)

include_directories(
	src/
)

add_executable(
	wandler
		src/wandler.cc
		src/camera.cc
		src/dep/imgui/imgui.cpp
		src/dep/imgui/imgui_draw.cpp
		src/dep/imgui_glut/imgui_impl_glut.cpp
)

target_link_libraries(
	wandler
		GL
		GLU
		glut
		GLEW
)
