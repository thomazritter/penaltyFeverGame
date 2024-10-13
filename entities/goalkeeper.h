#ifndef GOALKEEPER_H
#define GOALKEEPER_H

#include "sprite.h"
#include <GLFW/glfw3.h>

class Goalkeeper
{
public:
    Sprite sprite;

    Goalkeeper();
    void setupSprite();
    void resetPositions();
};

Goalkeeper::Goalkeeper() {}

void Goalkeeper::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int idleGoalkeeperTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/goalkeeper/idle.png", imgWidth, imgHeight);
    sprite.setupSprite(idleGoalkeeperTexture, glm::vec3(400.0, 475.0, 0.0), glm::vec3(imgWidth * 2, imgHeight * 3, 1.0), 1, 1);
}

void Goalkeeper::resetPositions()
{
    sprite.position = glm::vec3(400.0, 475.0, 0.0);
    sprite.updateFrame(0);
}

#endif
