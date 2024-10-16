#ifndef ARROW_H
#define ARROW_H

#include "sprite.h"
#include <GLFW/glfw3.h>
#include <utils.h>

class Arrow
{
public:
    Sprite sprite;
    float arrowSpeed = 4.5f;
    bool isHorizontal;
    bool isMovingRightOrUp = true;

    Arrow(bool isHorizontal);
    void setupSprite();
    void move();
    void resetPositions();
};

Arrow::Arrow(bool isHorizontal) : isHorizontal(isHorizontal) {}

void Arrow::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int arrowTexture = loadTexture("/sprites/arrow/arrow.png", imgWidth, imgHeight);
    if (isHorizontal)
    {
        sprite.setupSprite(arrowTexture, glm::vec3(400.0, 300.0, 0.0), glm::vec3(imgWidth * 2, imgHeight * 2, 1.0), 1, 1);
    }
    else
    {
        sprite.angle = -90.0f;
        sprite.setupSprite(arrowTexture, glm::vec3(175.0, 482.0, 0.0), glm::vec3(imgWidth * 2, imgHeight * 2, 1.0), 1, 1);
    }
}

void Arrow::resetPositions()
{
    if (isHorizontal)
        sprite.position = glm::vec3(400.0, 300.0, 0.0);
    else
        sprite.position = glm::vec3(175.0, 482.0, 0.0);

    sprite.updateFrame(0);
}

void Arrow::move()
{
    if (isHorizontal)
    {
        // Movimenta a seta da esquerda para a direita
        if (isMovingRightOrUp)
            sprite.position.x += arrowSpeed;
        else
            sprite.position.x -= arrowSpeed;
        // Inverta a direção se atingir os limites
        if (sprite.position.x > goalLimits.rightTop.x * 1.1f)
            isMovingRightOrUp = false;
        if (sprite.position.x < goalLimits.leftBottom.x * 0.9f)
            isMovingRightOrUp = true;
    }
    else
    {
        // Movimenta a seta de cima para baixo
        if (isMovingRightOrUp)
            sprite.position.y += arrowSpeed;
        else
            sprite.position.y -= arrowSpeed;
        // Inverta a direção se atingir os limites
        if (sprite.position.y > goalLimits.rightTop.y * 1.1f)
            isMovingRightOrUp = false;
        if (sprite.position.y < goalLimits.leftBottom.y * 0.9f)
            isMovingRightOrUp = true;
    }
}

#endif
