#include "../Header/TextRenderer.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

unsigned int TextRenderer::createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    auto compileShader = [](GLenum type, const char* src) -> unsigned int {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, 0);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "Shader compile error: " << infoLog << std::endl;
        }
        return shader;
        };

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Program link error: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

TextRenderer::TextRenderer(int width, int height) : SCREEN_WIDTH(width), SCREEN_HEIGHT(height) {
    const char* vShader =
        "#version 330 core\n"
        "layout(location = 0) in vec4 vertex;\n"
        "out vec2 TexCoords;\n"
        "uniform mat4 projection;\n"
        "void main() { gl_Position = projection * vec4(vertex.xy, 0.0, 1.0); TexCoords = vertex.zw; }";

    const char* fShader =
        "#version 330 core\n"
        "in vec2 TexCoords;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "void main() { float alpha = texture(text, TexCoords).r; FragColor = vec4(textColor, alpha); }";

    shaderID = createShaderProgram(vShader, fShader);

    float projection[16] = {
        2.0f / width, 0, 0, 0,
        0, 2.0f / height, 0, 0,
        0, 0, -1, 0,
       -1, -1, 0, 1
    };

    glUseProgram(shaderID);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, projection);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextRenderer::~TextRenderer() {
    for (auto& c : Characters) {
        glDeleteTextures(1, &c.second.TextureID);
    }
}

void TextRenderer::LoadFont(const char* fontPath, int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR: Could not init FreeType\n";
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath, 0, &face)) {
        std::cerr << "ERROR: Failed to load font\n";
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Failed to load Glyph " << c << "\n";
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
            face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            (int)face->glyph->bitmap.width,
            (int)face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, float r, float g, float b) {
    GLboolean depth = glIsEnabled(GL_DEPTH_TEST);
    if (depth) glDisable(GL_DEPTH_TEST);

    glUseProgram(shaderID);
    glUniform3f(glGetUniformLocation(shaderID, "textColor"), r, g, b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (char c : text) {
        auto it = Characters.find(c);
        if (it == Characters.end()) continue;
        Character ch = it->second;

        float xpos = x + ch.BearingX * scale;
        float ypos = y + (ch.BearingY - ch.SizeY) * scale;

        float w = ch.SizeX * scale;
        float h = ch.SizeY * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos,     ypos,     0.0f, 1.0f },
            { xpos + w, ypos,     1.0f, 1.0f },

            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos,     1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (depth) glEnable(GL_DEPTH_TEST);
}

void TextRenderer::RenderTextDownRight(std::string text, float scale, float r, float g, float b, float margin) {
    float textWidth = 0.0f;
    for (char c : text) {
        auto it = Characters.find(c);
        if (it == Characters.end()) continue;
        textWidth += (it->second.Advance >> 6) * scale;
    }

    float x = SCREEN_WIDTH - textWidth - margin * 2.0f;
    float y = margin;

    RenderText(text, x, y, scale, r, g, b);
}

void TextRenderer::RenderTextDownLeft(std::string text, float scale, float r, float g, float b, float margin) {
    float x = margin;
    float y = margin;
    RenderText(text, x, y, scale, r, g, b);
}

void TextRenderer::RenderTextTopRight(std::string text, float scale, float r, float g, float b, float margin) {
    float textWidth = 0.0f;
    for (char c : text) {
        auto it = Characters.find(c);
        if (it == Characters.end()) continue;
        textWidth += (it->second.Advance >> 6) * scale;
    }

    float x = SCREEN_WIDTH - textWidth - margin; // 10 px od desnog kraja
    float y = SCREEN_HEIGHT - margin;           // 10 px od vrha

    RenderText(text, x, y, scale, r, g, b);
}

void TextRenderer::RenderTextTopLeft(std::string text, float scale, float r, float g, float b, float margin) {
    float x = margin;           // 10 px od leve ivice
    float y = SCREEN_HEIGHT - 5.0f * margin; // 10 px od vrha

    RenderText(text, x, y, scale, r, g, b);
}

void TextRenderer::SetScreenSize(int width, int height) {
    float projection[16] = {
        2.0f / width, 0, 0, 0,
        0, 2.0f / height, 0, 0,
        0, 0, -1, 0,
       -1, -1, 0, 1
    };

    glUseProgram(shaderID);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, projection);
}
