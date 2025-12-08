#include "../Header/Station.h"
#include "../Header/TextRenderer.h"
#include <cmath>
#include <GL/glew.h>
#include <iostream>
#include <vector>

static unsigned int circleVAO = 0;
static unsigned int circleVBO = 0;
static const int NUM_SEGMENTS = 30;

void initStationRenderer() {
    if (circleVAO != 0) return;

    std::vector<float> vertices;
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= NUM_SEGMENTS; i++) {
        float theta = 2.0f * 3.1415926f * i / NUM_SEGMENTS;
        vertices.push_back(cosf(theta));
        vertices.push_back(sinf(theta));
    }

    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void drawStation(const Station& s, TextRenderer& textRenderer) {
    extern unsigned int circleShaderID;

    glUseProgram(circleShaderID);
    glUniform2f(glGetUniformLocation(circleShaderID, "uPosition"), s.x, s.y);
    glUniform1f(glGetUniformLocation(circleShaderID, "uScale"), 15.0f);
    glUniform3f(glGetUniformLocation(circleShaderID, "uColor"), 1.0f, 0.0f, 0.0f);

    glBindVertexArray(circleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SEGMENTS + 2);
    glBindVertexArray(0);

    textRenderer.RenderText(std::to_string(s.id), s.x - 5, s.y + 5, 1.0f, 1.0f, 1.0f, 1.0f);
}
