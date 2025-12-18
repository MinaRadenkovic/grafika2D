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

void drawStationCircle(const StationExtended& s, unsigned int shaderID);
void drawCurvedPath(const std::vector<StationExtended>& stations, unsigned int shaderID);

