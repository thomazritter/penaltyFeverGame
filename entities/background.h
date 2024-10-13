#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "sprite.h"
#include <GLFW/glfw3.h>

class Background
{
public:
    Sprite sprite;

    Background();
    void setupSprite();
};

Background::Background() {}

void Background::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int bgTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/background/movement.png", imgWidth, imgHeight);
    sprite.setupSprite(bgTexture, glm::vec3(400.0, 300.0, 0.0), glm::vec3(800.0, 600.0, 1.0), 2, 1);
}

#endif
