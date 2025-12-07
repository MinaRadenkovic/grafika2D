#include "../Header/Station.h"
#include "../Header/TextRenderer.h"
#include <cmath>
#include <GL/glew.h>
#include <iostream>
#include <vector>

static unsigned int circleVAO = 0;
static unsigned int circleVBO = 0;
static const int NUM_SEGMENTS = 30;

// Kreiranje VAO/VBO za krug (unit circle centered at 0,0)
void initStationRenderer() {
    if (circleVAO != 0) return; // vec inicijalizovano

    std::vector<float> vertices;
    // centar kruga
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

// Crta stanicu krugom (preko VAO/VBO) i tekstom
void drawStation(const Station& s, TextRenderer& textRenderer) {
    // Aktiviraj shader koji crta 2D krugove
    // Pretpostavljam da imas jednostavan shader za boju kruga
    // Primer uniform "uColor" i "uPosition" i "uScale"

    extern unsigned int circleShaderID; // shader mora biti kreiran u main.cpp

    glUseProgram(circleShaderID);
    glUniform2f(glGetUniformLocation(circleShaderID, "uPosition"), s.x, s.y);
    glUniform1f(glGetUniformLocation(circleShaderID, "uScale"), 15.0f); // radius
    glUniform3f(glGetUniformLocation(circleShaderID, "uColor"), 1.0f, 0.0f, 0.0f); // crvena

    glBindVertexArray(circleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SEGMENTS + 2);
    glBindVertexArray(0);

    // Crtaj broj u sredini
    textRenderer.RenderText(std::to_string(s.id), s.x - 5, s.y + 5, 1.0f, 1.0f, 1.0f, 1.0f);
}

// Kreira stanicu
std::vector<Station> createStations() {
    return {
        {0, 100, 100}, {1, 200, 150}, {2, 300, 120}, {3, 400, 200}, {4, 500, 180},
        {5, 600, 250}, {6, 700, 220}, {7, 650, 100}, {8, 550, 50}, {9, 350, 80}
    };
}
