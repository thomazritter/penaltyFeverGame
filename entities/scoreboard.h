#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "sprite.h"
#include <GLFW/glfw3.h>

class Scoreboard
{
public:
    Sprite sprite;

    Scoreboard();
    void setupSprite();
};

Scoreboard::Scoreboard() {}

void Scoreboard::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int bgTexture = loadTexture("/sprites/scoreboard/scoreboard.png", imgWidth, imgHeight);
    sprite.setupSprite(bgTexture, glm::vec3(610.0, 150.0, 0.0), glm::vec3(imgWidth / 2, imgHeight * 5, 1.0), 10, 1);
}

#endif
