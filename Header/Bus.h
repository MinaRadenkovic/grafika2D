#pragma once
#include <vector>
#include "Station.h"

enum BusState { MOVING, STOPPED };

struct Bus {
    float x, y;
    int currentSegment;
    float progress;
    float speed;
    BusState state;
    float stopTimer;
    bool hasInspector;
};

std::vector<Bus> createBuses();
void updateBuses(std::vector<Bus>& buses, const std::vector<Station>& stations);
void drawScene(const std::vector<Bus>& buses, const std::vector<Station>& stations);
