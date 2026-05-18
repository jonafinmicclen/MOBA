#pragma once
#include <glad/glad.h>
#include <iostream>
#include <string>

GLuint compileShader(GLenum type, const char* source);
GLuint createShaderProgram(const char* vertSrc, const char* fragSrc);
std::string readFileToString(const std::string& path);
