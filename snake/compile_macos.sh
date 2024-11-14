#!/bin/bash

PROJECT_DIR=$(pwd)
BUILD_DIR="${PROJECT_DIR}/bin"

# third-party libraries
GLAD_DIR="/usr/local/include/glad"
GLAD_INCLUDE="${GLAD_DIR}/include"
GLAD_SRC="${GLAD_DIR}/src/glad.c"
GLM_INCLUDE="$(brew --prefix glm)/include"
GLFW_INCLUDE="$(brew --prefix glfw)/include"
GLFW_LIBS="$(brew --prefix glfw)/lib"

SRC_FILES="${PROJECT_DIR}/main.cpp ${GLAD_SRC}"
EXE_NAME="snake"

# Step 2: Create the build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir "$BUILD_DIR"
fi

clang++ -std=c++11 -o "$BUILD_DIR/$EXE_NAME" $SRC_FILES \
    -I$GLFW_INCLUDE -I$GLM_INCLUDE -I$GLAD_INCLUDE -framework OpenGL \
    -L$GLFW_LIBS -lglfw