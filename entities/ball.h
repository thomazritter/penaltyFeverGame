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
    void setTarget(GoalSection section);
    void moveBall(bool &isBallAnimationComplete);
    void resetPositions();

private:
    Coordinates kickTarget;
    float totalBallDistance = 0.0f;
    float ballSpeed;
};

Ball::Ball() {}

void Ball::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int ballTexture = loadTexture("/sprites/ball/movement.png", imgWidth, imgHeight);
    sprite.setupSprite(ballTexture, glm::vec3(400.0, 200.0, 0.0), glm::vec3(imgWidth / 3.0 * 3.0, imgHeight * 2, 1.5), 3, 1);
}

void Ball::moveBall(bool &isBallAnimationComplete)
{
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - sprite.lastTime;

    if (deltaTime >= 0.5f / sprite.FPS) // Update sprite each 0.5 seconds
    {
        // Calculate the total distance between the ball's current position and the target
        float dx = kickTarget.x - sprite.position.x;
        float dy = kickTarget.y - sprite.position.y;
        float distance = sqrt(dx * dx + dy * dy); // Hypotenuse

        // Only move the ball if the distance is greater than a small threshold
        if (distance > ballSpeed)
        {
            // Normalize the direction vector by dividing by the distance (hypotenuse)
            float directionX = dx / distance;
            float directionY = dy / distance;

            // Move the ball by a fixed step size
            float stepSize = ballSpeed; // Assuming a constant frame time of 16ms (~60 FPS)
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

        sprite.lastTime = currentTime;
    }
}

void Ball::resetPositions()
{
    sprite.position = glm::vec3(400.0, 200.0, 0.0);
    sprite.updateFrame(0);
}

void Ball::setTarget(GoalSection section)
{
    kickTarget = determineTargetCoordinates(section);
    totalBallDistance = sqrt(pow(kickTarget.x - sprite.position.x, 2) + pow(kickTarget.y - sprite.position.y, 2));
    ballSpeed = totalBallDistance / (sprite.FPS * 2); // 24 steps to reach the target
}

#endif
