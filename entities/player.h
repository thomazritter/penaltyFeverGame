#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"
#include <GLFW/glfw3.h>

class Player
{
public:
    Sprite sprite;

    Player();
    void setupSprite();
    void movePlayer(bool &isKickAnimationComplete);
    void resetPositions();
};
Player::Player() {}

void Player::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int playerTexture = loadTexture("/sprites/player/movement.png", imgWidth, imgHeight);
    sprite.setupSprite(playerTexture, glm::vec3(300.0, 175.0, 0.0), glm::vec3(imgWidth / 2, imgHeight * 2.0, 1.0), 6, 1);
}

void Player::movePlayer(bool &isKickAnimationComplete)
{
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - sprite.lastTime;

    if (deltaTime >= 1.5 / sprite.FPS)
    {
        switch (sprite.iFrame)
        {
        case 4:
            sprite.updateFrame(5);
            sprite.position = glm::vec3(400.0f, 220.0f, 0.0f);
            break;
        case 3:
            sprite.updateFrame(4);
            sprite.position = glm::vec3(380.0f, 210.0f, 0.0f);
            break;
        case 2:
            sprite.updateFrame(3);
            sprite.position = glm::vec3(360.0f, 190.0f, 0.0f);
            break;
        case 1:
            sprite.updateFrame(2);
            sprite.position = glm::vec3(340.0f, 185.0f, 0.0f);
            break;
        case 0:
            sprite.updateFrame(1);
            sprite.position = glm::vec3(320.0f, 180.0f, 0.0f);
            break;
        }
        sprite.lastTime = currentTime;
    }

    if (sprite.iFrame == 5)
    {
        isKickAnimationComplete = true;
    }
}

void Player::resetPositions()
{
    sprite.position = glm::vec3(300.0, 175.0, 0.0);
    sprite.updateFrame(0);
}

#endif
