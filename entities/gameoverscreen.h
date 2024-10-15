#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include "sprite.h"
#include <GLFW/glfw3.h>

class GameOverScreen
{
public:
    Sprite sprite;

    GameOverScreen();
    void setupSprite();
};

GameOverScreen::GameOverScreen() {}

void GameOverScreen::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int gameoverTexture = loadTexture("/sprites/ending/ending.png", imgWidth, imgHeight);
    sprite.setupSprite(gameoverTexture, glm::vec3(400.0, 300.0, 0.0), glm::vec3(800.0, 600.0, 1.0), 2, 1);
}

#endif
