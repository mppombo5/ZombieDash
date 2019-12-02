cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME "ZombieDash")
# set your directories for freeGLUT here
set(GLUT_INCLUDE_DIR /usr/local/include)
set(GLUT_LIB_DIR /usr/local/lib)
set(GLUT_LIB_FILES
        ${GLUT_LIB_DIR}/libglut.3.10.0.dylib
        ${GLUT_LIB_DIR}/libglut.3.dylib
        ${GLUT_LIB_DIR}/libglut.a
        ${GLUT_LIB_DIR}/libglut.dylib
        )

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

add_executable(ZombieDash src/provided/main.cpp src/Actor.cpp src/provided/GameController.cpp src/provided/GameWorld.cpp src/StudentWorld.cpp)

include_directories(${GLUT_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${GLUT_LIB_DIR})