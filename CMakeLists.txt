cmake_minimum_required(VERSION 3.12.0)
project(WINDOW_GUI_TEST VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)


file(GLOB WINDOW_GUI_TEST_SRC CONFIGURE_DEPENDS "./src/*.h" "./src/*.cpp")

add_executable(WINDOW_GUI_TEST ${WINDOW_GUI_TEST_SRC})
target_link_libraries(WINDOW_GUI_TEST
	glfw
	GLEW
    GLU
    GL
    assimp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
