#include "../Header/Bus.h"
#include "../Header/Station.h"
#include <GL/glew.h>

std::vector<Bus> createBuses()
{
    return {
        { -0.8f, -0.2f, 0, 0.0f, 0.25f, MOVING, 0.0f, false },
        { -0.8f, -0.2f, 0, 0.0f, 0.18f, MOVING, 0.0f, true }
    };
}


void updateBuses(std::vector<Bus>& buses, const std::vector<Station>& stations)
{
    float dt = 0.016f; // ~60 FPS

    for (auto& bus : buses)
    {
        if (bus.state == STOPPED)
        {
            bus.stopTimer -= dt;
            if (bus.stopTimer <= 0)
                bus.state = MOVING;
            continue;
        }

        if (bus.currentSegment >= stations.size() - 1)
            continue;

        bus.progress += bus.speed * dt;
        if (bus.progress >= 1.0f)
        {
            bus.progress = 0.0f;
            bus.currentSegment++;
            bus.state = STOPPED;
            bus.stopTimer = bus.hasInspector ? 3.0f : 1.5f;
        }

        // Interpolacija pozicije
        Station a = stations[bus.currentSegment];
        Station b = stations[(bus.currentSegment + 1) % stations.size()];
        bus.x = a.x + (b.x - a.x) * bus.progress;
        bus.y = a.y + (b.y - a.y) * bus.progress;
    }
}


void drawCircle(float cx, float cy, float r)
{
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 32; i++)
    {
        float a = i * 0.19635f;
        glVertex2f(cx + cos(a) * r, cy + sin(a) * r);
    }
    glEnd();
}


void drawScene(const std::vector<Bus>& buses, const std::vector<Station>& stations)
{
    // Stanice
    glColor3f(0.1f, 0.1f, 0.1f);
    for (auto& st : stations)
        drawCircle(st.x, st.y, 0.03f);

    // Autobusi
    for (auto& b : buses)
    {
        if (b.hasInspector)
            glColor3f(1.0f, 0.3f, 0.3f); // bus sa kontrolom
        else
            glColor3f(0.2f, 0.2f, 1.0f);

        drawCircle(b.x, b.y, 0.04f);
    }
}
