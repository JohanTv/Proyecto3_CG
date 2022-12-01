cmake_minimum_required(VERSION 3.19)
project(Superficie)

set(CMAKE_CXX_STANDARD 14)

add_executable(Superficie main.cpp glad.c)

if(UNIX AND NOT APPLE)
    find_package(GLEW REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} OpenGL::GL GLUT::GLUT GLEW::GLEW glfw ${CMAKE_DL_LIBS})
else()
    target_link_libraries(${PROJECT_NAME} -lOpenGL32 -lGLU32 -lfreeGLUT -lglfw3dll)
endif()
