#include "../Header/Bus.h"
#include "../Header/Station.h"
#include "../Header/Util.h"
#include <GL/glew.h>
#include <random>

void updateBus(Bus& bus, const std::vector<StationExtended>& stations, float deltaTime)
{
    const float MOVE_SPEED = 0.8f;
    const float STOP_DURATION = 10.0f;

    if (stations.size() < 2) return;
    if (!bus.atStation) {
        bus.t += MOVE_SPEED * deltaTime;
        if (bus.t >= 1.0f) {
            bus.t = 0.0f;
            bus.currentSegment++;

            if (bus.currentSegment >= (int)stations.size() - 1)
                bus.currentSegment = (int)stations.size() - 2;

            bus.atStation = true;
            bus.stopTime = 0.0f;
        }
        return;
    }

    bus.stopTime += deltaTime;
    const StationExtended& st = stations[bus.currentSegment];

    if (bus.hasInspector && bus.currentSegment == bus.inspectorExitSegment) {
        bus.hasInspector = false;
        if (bus.passengers > 1) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, bus.passengers - 1);

            int fines = dist(gen);
            bus.finesCollected += fines;
        }
        bus.passengers -= 1;
    }

    if (bus.stopTime *10>= STOP_DURATION) {
        bus.atStation = false;
        bus.t = 0.0f;
    }
}

void drawBus(const Bus& bus, const std::vector<StationExtended>& stations, unsigned int textureShaderID) {
    StationExtended s0 = stations[bus.currentSegment];
    StationExtended s1 = stations[(bus.currentSegment + 1) % stations.size()];

    float x = s0.normX * (1 - bus.t) + s1.normX * bus.t;
    float y = s0.normY * (1 - bus.t) + s1.normY * bus.t;

    float ndcX = x * 2.0f - 1.0f;
    float ndcY = 1.0f - y * 2.0f + 0.1f;

    float size = 0.25f;
    float vertices[] = {
        ndcX - size, ndcY - size, 0.0f, 0.0f,
        ndcX + size, ndcY - size, 1.0f, 0.0f,
        ndcX + size, ndcY + size, 1.0f, 1.0f,
        ndcX - size, ndcY + size, 0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(textureShaderID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bus.textureID);
    glUniform1i(glGetUniformLocation(textureShaderID, "uTexture"), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    if (bus.hasInspector) {
        float size = 0.15f;
        float margin = 0.05f;
        float ndcX = 1.0f - size - margin;
        float ndcY = 1.0f - size - margin;

        float vertices[] = {
            ndcX - size, ndcY - size, 0.0f, 0.0f,
            ndcX + size, ndcY - size, 1.0f, 0.0f,
            ndcX + size, ndcY + size, 1.0f, 1.0f,
            ndcX - size, ndcY + size, 0.0f, 1.0f
        };
        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        unsigned int VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glUseProgram(textureShaderID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bus.inspectorTex);
        glUniform1i(glGetUniformLocation(textureShaderID, "uTexture"), 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
}

void drawBusDoor(const Bus& bus, unsigned int shaderID)
{
    float margin = 0.05f;
    float height = 0.25f;
    float width = height * 0.75f;

    float ndcX = -1.0f + margin + width;
    float ndcY = -1.0f + margin + height;

    float vertices[] = {
        ndcX - width, ndcY - height, 0.0f, 0.0f,
        ndcX + width, ndcY - height, 1.0f, 0.0f,
        ndcX + width, ndcY + height, 1.0f, 1.0f,
        ndcX - width, ndcY + height, 0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bus.atStation ? bus.doorOpenTex : bus.doorClosedTex);
    glUniform1i(glGetUniformLocation(shaderID, "uTexture"), 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}
