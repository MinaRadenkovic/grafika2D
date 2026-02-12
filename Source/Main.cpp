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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int circleShaderID;
unsigned int lineShaderID;
unsigned int busShaderID;
unsigned int windshieldShader;
int windowWidth = 0;
int windowHeight = 0;
unsigned int windshieldVAO, windshieldVBO;
unsigned int panelFBO;
unsigned int panelTexture;

float yaw = 0.0f;
float pitch = 0.0f;

float lastX = windowWidth / 2;
float lastY = windowHeight / 2;
bool firstMouse = true;

glm::vec3 cameraPos = glm::vec3(0.0f, 1.5f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Pitch limit
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Yaw limit (180° total)
    if (yaw > 90.0f) yaw = 90.0f;
    if (yaw < -90.0f) yaw = -90.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw - 90.0f)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw - 90.0f)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (glewInit() != GLEW_OK)
        return endProgram("GLEW nije uspeo da se inicijalizuje.");

	glEnable(GL_DEPTH_TEST);
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
    windshieldShader = createShader("Source/windshield.vert", "Source/windshield.frag");

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

    unsigned int roadTex = loadImageToTexture("Resources/road.jpg");
    glBindTexture(GL_TEXTURE_2D, roadTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    float windshieldVertices[] = {

        -2.0f,  2.5f, -3.0f,     0.0f, 1.0f,
        -2.0f,  0.5f, -3.0f,     0.0f, 0.0f,
         2.0f,  0.5f, -3.0f,     1.0f, 0.0f,

        -2.0f,  2.5f, -3.0f,     0.0f, 1.0f,
         2.0f,  0.5f, -3.0f,     1.0f, 0.0f,
         2.0f,  2.5f, -3.0f,     1.0f, 1.0f
    };

    glGenVertexArrays(1, &windshieldVAO);
    glGenBuffers(1, &windshieldVBO);

    glBindVertexArray(windshieldVAO);

    glBindBuffer(GL_ARRAY_BUFFER, windshieldVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windshieldVertices),
        windshieldVertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    float panelVertices[] = {
        // pozicija             // tex
        -1.5f, -0.5f, -2.0f,     0.0f, 1.0f,
        -1.5f, -1.5f, -2.0f,     0.0f, 0.0f,
         1.5f, -1.5f, -2.0f,     1.0f, 0.0f,

        -1.5f, -0.5f, -2.0f,     0.0f, 1.0f,
         1.5f, -1.5f, -2.0f,     1.0f, 0.0f,
         1.5f, -0.5f, -2.0f,     1.0f, 1.0f
    };

    glGenFramebuffers(1, &panelFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, panelFBO);

    glGenTextures(1, &panelTexture);
    glBindTexture(GL_TEXTURE_2D, panelTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        1024, 1024,
        0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        panelTexture,
        0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "FBO nije kompletan!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //3D;
        glUseProgram(windshieldShader);

        // perspektiva
        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            (float)windowWidth / windowHeight,
            0.1f, 100.0f
        );

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );

        glUniformMatrix4fv(glGetUniformLocation(windshieldShader, "projection"),
            1, GL_FALSE, &projection[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(windshieldShader, "view"),
            1, GL_FALSE, &view[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, roadTex);
        glUniform1i(glGetUniformLocation(windshieldShader, "tex"), 0);

        // blend ON
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(windshieldVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);



        // 2D

		glDisable(GL_DEPTH_TEST);


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

		glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (myCursor) glfwDestroyCursor(myCursor);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
