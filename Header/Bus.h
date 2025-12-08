#pragma once
#include <vector>
#include "Station.h"

struct Bus {
    unsigned int textureID;
    unsigned int doorClosedTex;
    unsigned int doorOpenTex;
    float t = 0.0f;
    int currentSegment = 0;
    bool hasInspector = false;
    bool atStation = false;
    float stopTime = 0.0f;
	int passengers = 0;
    int finesCollected = 0;
    unsigned int inspectorTex;
    int inspectorExitSegment = -1;
    int inspectorEntrySegment = -1;
};

void updateBus(Bus& bus, const std::vector<StationExtended>& stations, float deltaTime);
void drawBus(const Bus& bus, const std::vector<StationExtended>& stations, unsigned int shaderID);
void drawBusDoor(const Bus& bus, unsigned int shaderID);
