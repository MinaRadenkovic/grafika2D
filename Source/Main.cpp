#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Header/Util.h"
#include "../Header/Station.h"
#include "../Header/Bus.h"
#include "../Header/TextRenderer.h"
#include <iostream>
#include <vector>
#include <cmath>
#include "../Header/stb_image.h"

unsigned int circleShaderID;
unsigned int lineShaderID;
unsigned  int busShaderID;
int windowWidth = 0;
int windowHeight = 0;


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action != GLFW_PRESS) return;

    Bus* bus = reinterpret_cast<Bus*>(glfwGetWindowUserPointer(window));
    if (!bus) return;

    if (!bus->atStation) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (bus->passengers < 50) bus->passengers++;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (bus->passengers > 0) bus->passengers--;
    }
}


int main() {
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

    GLFWcursor* myCursor = loadImageToCursor("Resources/bus_stop_sign.png");
    if (myCursor)
        glfwSetCursor(window, myCursor);

    TextRenderer textRenderer(windowWidth, windowHeight);
    textRenderer.LoadFont("Resources/Arimo-Regular.ttf", 48);

    circleShaderID = createShader("Source/circleShader.vert", "Source/circleShader.frag");
    lineShaderID = createShader("Source/lineShader.vert", "Source/lineShader.frag");
    busShaderID = createShader("Source/textureShader.vert", "Source/textureShader.frag");

    std::vector<StationExtended> stations = {
    {0, 100, 100}, {1, 200, 150}, {2, 300, 120}, {3, 400, 200},
    {4, 500, 180}, {5, 600, 250}, {6, 700, 220}, {7, 650, 100},
    {8, 550, 50},  {9, 350, 80}
    };

    float minX = 1e9, minY = 1e9;
    float maxX = -1e9, maxY = -1e9;
    for (auto& s : stations) {
        if (s.x < minX) minX = s.x;
        if (s.x > maxX) maxX = s.x;
        if (s.y < minY) minY = s.y;
        if (s.y > maxY) maxY = s.y;
    }
    float rawW = maxX - minX;
    float rawH = maxY - minY;

    float margin = 0.1f;
    for (auto& s : stations) {
        s.normX = margin + (s.x - minX) / rawW * (1.0f - 2 * margin);
        s.normY = margin + (s.y - minY) / rawH * (1.0f - 2 * margin);
    }

    Bus bus;
    unsigned int infoTex = loadImageToTexture("Resources/bus.png");
    glBindTexture(GL_TEXTURE_2D, infoTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    bus.textureID = infoTex;

    unsigned int doorClosedTex = loadImageToTexture("Resources/door_closed.png");
    glBindTexture(GL_TEXTURE_2D, doorClosedTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int doorOpenTex = loadImageToTexture("Resources/door_opened.png");
    glBindTexture(GL_TEXTURE_2D, doorOpenTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int inspectorTexID = loadImageToTexture("Resources/controller.jpg");
    glBindTexture(GL_TEXTURE_2D, inspectorTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    bus.doorClosedTex = doorClosedTex;
    bus.doorOpenTex = doorOpenTex;
	bus.inspectorTex = inspectorTexID;

    glfwSetWindowUserPointer(window, &bus);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    double lastTime = glfwGetTime();
    const double TARGET_FPS = 75.0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        if (bus.atStation && glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !bus.hasInspector) {
            bus.hasInspector = true;
            bus.passengers += 1;
            bus.inspectorExitSegment = (bus.currentSegment + 1) % stations.size();
        }

        limitFramesPerSecond(TARGET_FPS, lastTime);
        glClear(GL_COLOR_BUFFER_BIT);

        drawCurvedPath(stations, lineShaderID);

        for (auto& s : stations) {
            drawStationCircle(s, circleShaderID);
            float textX = s.normX * windowWidth - 10;
            float textY = (1.0f - s.normY) * windowHeight - 10;
            textRenderer.RenderText(std::to_string(s.id), textX, textY, 1.0f, 1, 1, 1);
        }

        updateBus(bus, stations, deltaTime);
        drawBus(bus, stations, busShaderID);
        drawBusDoor(bus, busShaderID);

        std::string infoText = "Putnici: " + std::to_string(bus.passengers) +
            "    Kazne: " + std::to_string(bus.finesCollected);        
        textRenderer.RenderTextDownRight(infoText, 1.0f, 1.0f, 1.0f, 1.0f, 10.0f);
        textRenderer.RenderTextTopLeft("Mina Radenkovic SV76/2022", 1.0f, 1.0f, 1.0f, 1.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (myCursor) glfwDestroyCursor(myCursor);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
