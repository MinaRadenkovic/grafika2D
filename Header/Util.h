#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
int endProgram(std::string message);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned loadImageToTexture(const char* filePath);
GLFWcursor* loadImageToCursor(const char* filePath);

void limitFramesPerSecond(double targetFps, double& lastTime);
unsigned int compileShaderFromSource(GLenum type, const char* sourceCode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
