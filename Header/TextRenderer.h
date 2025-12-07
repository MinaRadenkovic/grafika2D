#pragma once
#include <GL/glew.h>
#include <string>
#include <map>

struct Character {
    unsigned int TextureID;
    int SizeX;
    int SizeY;
    int BearingX;
    int BearingY;
    unsigned int Advance;
};

class TextRenderer {
public:
    TextRenderer(int width, int height);
    ~TextRenderer();

    void LoadFont(const char* fontPath, int fontSize);
    void RenderText(std::string text, float x, float y, float scale, float r, float g, float b);
    void RenderTextDownRight(std::string text, float scale, float r, float g, float b, float margin = 10.0f);
    void RenderTextDownLeft(std::string text, float scale, float r, float g, float b, float margin = 10.0f);
    void RenderTextTopRight(std::string text, float scale, float r, float g, float b, float margin = 10.0f);
    void RenderTextTopLeft(std::string text, float scale, float r, float g, float b, float margin = 10.0f);


private:
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;
    unsigned int shaderID;
    int SCREEN_WIDTH, SCREEN_HEIGHT;

    unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc);
};
