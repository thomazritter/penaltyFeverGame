#ifndef BALL_H
#define BALL_H

#include "sprite.h"
#include <GLFW/glfw3.h>
#include <utils.h>

class Ball
{
public:
    Sprite sprite;

    Ball();
    void setupSprite();
    void moveBall(bool &isBallAnimationComplete, Coordinates kickTarget, float ballSpeed, float totalBallDistance);
    void resetPositions();
};

Ball::Ball() {}

void Ball::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int ballTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/ball/movement.png", imgWidth, imgHeight);
    sprite.setupSprite(ballTexture, glm::vec3(400.0, 200.0, 0.0), glm::vec3(imgWidth / 3.0 * 3.0, imgHeight * 2, 1.5), 3, 1);
}

void Ball::moveBall(bool &isBallAnimationComplete, Coordinates kickTarget, float ballSpeed, float totalBallDistance)
{
    // Calculate the total distance between the ball's current position and the target
    float dx = kickTarget.x - sprite.position.x;
    float dy = kickTarget.y - sprite.position.y;
    float distance = sqrt(dx * dx + dy * dy); // Hypotenuse

    // Only move the ball if the distance is greater than a small threshold
    if (distance > 3.0f)
    {
        // Normalize the direction vector by dividing by the distance (hypotenuse)
        float directionX = dx / distance;
        float directionY = dy / distance;

        // Move the ball by a fixed step size
        float stepSize = ballSpeed * 0.032f; // Assuming a constant frame time of 16ms (~60 FPS)
        sprite.position.x += directionX * stepSize;
        sprite.position.y += directionY * stepSize;

        // Update the animation frame based on the distance traveled
        if (distance < totalBallDistance / 3)
        {
            sprite.updateFrame(2);
        }
        else if (distance < totalBallDistance / 3 * 2)
        {
            sprite.updateFrame(1);
        }
        else
        {
            sprite.updateFrame(0);
        }
    }
    else
    {
        sprite.position.x = kickTarget.x;
        sprite.position.y = kickTarget.y;
        isBallAnimationComplete = true;
    }
}

void Ball::resetPositions()
{
    sprite.position = glm::vec3(400.0, 200.0, 0.0);
    sprite.updateFrame(0);
}

#endif
