#pragma once
#include <vector>
#include <GL/glew.h>
#include <string>

struct Station {
    int id;
    float x, y;
};

struct StationExtended : public Station {
    float normX = 0.0f;
    float normY = 0.0f;
    StationExtended(int _id, float _x, float _y) : Station{ _id, _x, _y } {}
};

void initStationRenderer();
void drawStation(const Station& s, class TextRenderer& textRenderer);

