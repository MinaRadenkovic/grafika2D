#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Header/Util.h"
#include "../Header/Station.h"
#include "../Header/Bus.h"
#include "../Header/TextRenderer.h"
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>

// Main fajl funkcija sa osnovnim komponentama OpenGL programa

unsigned int circleShaderID;
int windowWidth = 0;
int windowHeight = 0;

void initStationRenderer();

// Funkcija koja crta linije izmedju stanica (putanja)
void drawPath(const std::vector<Station>& stations, unsigned int shaderID)
{
    glLineWidth(10.0f);  // debljina putanje

    glUseProgram(shaderID);
    glUniform3f(glGetUniformLocation(shaderID, "uColor"), 1.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < stations.size(); ++i)
    {
        // konvertuj u NDC
        float x0 = (stations[i].x / (float)windowWidth) * 2.0f - 1.0f;
        float y0 = (stations[i].y / (float)windowHeight) * 2.0f - 1.0f;
        float x1 = (stations[(i + 1) % stations.size()].x / (float)windowWidth) * 2.0f - 1.0f;
        float y1 = (stations[(i + 1) % stations.size()].y / (float)windowHeight) * 2.0f - 1.0f;

        float lineVerts[4] = { x0, y0, x1, y1 };

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVerts), lineVerts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, 2);

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }
}

// Funkcija koja crta krugove za stanice
void drawStationCircle(const Station& s, unsigned int shaderID)
{
    const int NUM_SEGMENTS = 30;
    float circleVerts[NUM_SEGMENTS * 2];

    for (int i = 0; i < NUM_SEGMENTS; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(NUM_SEGMENTS);
        circleVerts[i * 2] = cos(theta);
        circleVerts[i * 2 + 1] = sin(theta);
    }

    glUseProgram(shaderID);
    glUniform3f(glGetUniformLocation(shaderID, "uColor"), 1.0f, 0.0f, 0.0f);

    float ndcX = (s.x / (float)windowWidth) * 2.0f - 1.0f;
    float ndcY = (s.y / (float)windowHeight) * 2.0f - 1.0f;

    glUniform2f(glGetUniformLocation(shaderID, "uPosition"), ndcX, ndcY);
    glUniform1f(glGetUniformLocation(shaderID, "uScale"), 0.04f);

    unsigned int VBO, VAO;
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

int main()
{
    // Inicijalizacija GLFW
    if (!glfwInit())
        return endProgram("Ne mogu da inicijalizujem GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    windowWidth = mode->width;
    windowHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Autobusi", primary, NULL);
    if (!window)
        return endProgram("Prozor nije uspeo da se kreira.");

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return endProgram("GLEW nije uspeo da se inicijalizuje.");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.8f, 0.6f, 1.0f);
    glViewport(0, 0, windowWidth, windowHeight);

    // Cursor
    GLFWcursor* myCursor = loadImageToCursor("Resources/bus_stop_sign.png");
    if (myCursor)
        glfwSetCursor(window, myCursor);

    // Text renderer
    TextRenderer textRenderer(windowWidth, windowHeight);
    textRenderer.LoadFont("Resources/Arimo-Regular.ttf", 16);

    // Shaderi
    circleShaderID = createShader("Source/circleShader.vert", "Source/circleShader.frag");
    unsigned int lineShaderID = createShader("Source/lineShader.vert", "Source/lineShader.frag");

    initStationRenderer();

    // Stanice i autobusi
    std::vector<Station> stations = createStations();
    std::vector<Bus> buses = createBuses();

    double lastTime = glfwGetTime();
    const double TARGET_FPS = 75.0;

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        limitFramesPerSecond(TARGET_FPS, lastTime);

        glClear(GL_COLOR_BUFFER_BIT);

        // Crtanje puta
        drawPath(stations, lineShaderID);

        // Crtanje stanica
        for (size_t i = 0; i < stations.size(); ++i)
        {
            drawStationCircle(stations[i], circleShaderID);
            // Render text u NDC ili u "screen space" (ako textRenderer radi u pixels)
            textRenderer.RenderText(std::to_string(i), stations[i].x - 8, stations[i].y - 8, 1.0f, 1, 1, 1);
        }

        // Update i crtanje autobusa
        updateBuses(buses, stations);
        drawScene(buses, stations);

        // Tvoje ime u gornjem levom uglu
        textRenderer.RenderTextTopLeft("Mina Radenkovic SV76/2022", 1.0f, 1.0f, 1.0f, 1.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (myCursor) glfwDestroyCursor(myCursor);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
