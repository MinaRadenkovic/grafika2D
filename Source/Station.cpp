#include "../Header/Station.h"
#include "../Header/TextRenderer.h"
#include <cmath>
#include <GL/glew.h>
#include <iostream>
#include <vector>

static const int NUM_SEGMENTS = 30;
static unsigned int VAO, VBO;


void drawCurvedPath(const std::vector<StationExtended>& stations, unsigned int shaderID) {
    glLineWidth(10.0f);
    glUseProgram(shaderID);
    glUniform3f(glGetUniformLocation(shaderID, "uColor"), 1.0f, 0.0f, 0.0f);

    std::vector<float> verts;
    size_t n = stations.size();

    for (size_t i = 0; i < n; ++i) {
        StationExtended p0 = stations[(i + n - 1) % n];
        StationExtended p1 = stations[i];
        StationExtended p2 = stations[(i + 1) % n];
        StationExtended p3 = stations[(i + 2) % n];

        for (int t = 0; t <= 20; ++t) {
            float u = t / 20.0f;
            float u2 = u * u;
            float u3 = u2 * u;

            float x = 0.5f * ((2 * p1.normX) +
                (-p0.normX + p2.normX) * u +
                (2 * p0.normX - 5 * p1.normX + 4 * p2.normX - p3.normX) * u2 +
                (-p0.normX + 3 * p1.normX - 3 * p2.normX + p3.normX) * u3);

            float y = 0.5f * ((2 * p1.normY) +
                (-p0.normY + p2.normY) * u +
                (2 * p0.normY - 5 * p1.normY + 4 * p2.normY - p3.normY) * u2 +
                (-p0.normY + 3 * p1.normY - 3 * p2.normY + p3.normY) * u3);

            verts.push_back(x * 2.0f - 1.0f);
            verts.push_back(1.0f - y * 2.0f);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINE_STRIP, 0, verts.size() / 2);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}


void drawStationCircle(const StationExtended& s, unsigned int shaderID) {
    const int NUM_SEGMENTS = 30;
    float circleVerts[NUM_SEGMENTS * 2];

    for (int i = 0; i < NUM_SEGMENTS; i++) {
        float theta = 2.0f * 3.1415926f * i / NUM_SEGMENTS;
        circleVerts[i * 2] = cos(theta);
        circleVerts[i * 2 + 1] = sin(theta);
    }

    glUseProgram(shaderID);
    glUniform3f(glGetUniformLocation(shaderID, "uColor"), 1.0f, 0.0f, 0.0f);

    float ndcX = s.normX * 2.0f - 1.0f;
    float ndcY = 1.0f - s.normY * 2.0f;

    glUniform2f(glGetUniformLocation(shaderID, "uPosition"), ndcX, ndcY);
    glUniform1f(glGetUniformLocation(shaderID, "uScale"), 0.07f);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVerts), circleVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SEGMENTS);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}