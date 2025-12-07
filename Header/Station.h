#pragma once
#include <vector>
#include <GL/glew.h>
#include <string>

struct Station {
    int id;
    float x, y;
};

void initStationRenderer();

// Crta jednu stanicu (krug + tekst)
void drawStation(const Station& s, class TextRenderer& textRenderer);

// Kreira niz stanica sa koordinatama
std::vector<Station> createStations();
