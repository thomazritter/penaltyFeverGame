#ifndef GOALKEEPER_H
#define GOALKEEPER_H

#include "sprite.h"
#include <GLFW/glfw3.h>
#include <utils.h>
#include <random>

class Goalkeeper
{
public:
    Sprite sprite;
    glm::vec3 targetPosition;  // Target position within goal limits
    float moveSpeed = 3.0f;
    float totalMoveDistance = 0.0f;
    int currentFrameIndex = 0;
    int defenseType = 0;       // 0: Left Corner, 1: Right Corner, 2: Up

    Goalkeeper();
    void setupSprite();
    void moveGoalkeeper(bool &isGoalkeeperAnimationComplete, GoalLimits goalLimits);
    void resetPositions();
    float randomFloat(float min, float max);
    int chooseDefenseType(); // Chooses defense type randomly
};

Goalkeeper::Goalkeeper() {}

void Goalkeeper::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int goalkeeperTexture = loadTexture("/sprites/goalkeeper/movement.png", imgWidth, imgHeight);
    sprite.setupSprite(goalkeeperTexture, glm::vec3(400.0, 475.0, 0.0), glm::vec3(imgWidth / 3, imgHeight * 3.0, 1.0), 7, 1);
}

float Goalkeeper::randomFloat(float min, float max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return static_cast<float>(dis(gen));
}

// Choose defense type (0 = Left Bottom, 1 = Right Bottom, 2 = Up)
int Goalkeeper::chooseDefenseType()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2);
    return dis(gen);
}

void Goalkeeper::moveGoalkeeper(bool &isGoalkeeperAnimationComplete, GoalLimits goalLimits)
{
    static const float idleYPosition = sprite.position.y; // Fixed Y position for animations

    // Choose a random defense type if no target set
    if (totalMoveDistance == 0.0f)
    {
        defenseType = chooseDefenseType();

        // Set the target position based on defense type, keeping Y consistent for side defenses
        if (defenseType == 0) // Left Corner Bottom
        {
            targetPosition = glm::vec3(goalLimits.leftBottom.x, idleYPosition, sprite.position.z);
            currentFrameIndex = 2; // Start with frame 2 for bottom defense
        }
        else if (defenseType == 1) // Right Corner Bottom
        {
            targetPosition = glm::vec3(goalLimits.rightTop.x, idleYPosition, sprite.position.z);
            currentFrameIndex = 2; // Start with frame 2 for bottom defense
        }
        else if (defenseType == 2) // Up (Center High)
        {
            targetPosition = glm::vec3((goalLimits.leftBottom.x + goalLimits.rightTop.x) / 2.0f, goalLimits.rightTop.y, sprite.position.z);
            currentFrameIndex = 1; // Start with frame 1 for upward jump
        }

        // Calculate total distance to target for animation
        totalMoveDistance = glm::distance(sprite.position, targetPosition);
    }

    // Calculate direction and distance to target
    float dx = targetPosition.x - sprite.position.x;
    float dy = targetPosition.y - sprite.position.y;
    float distance = sqrt(dx * dx + dy * dy);

    // Move toward the target if not close enough
    if (distance > 3.0f)
    {
        // Normalize direction
        float directionX = dx / distance;
        float directionY = dy / distance;

        // Move goalkeeper by a fixed step size
        float stepSize = moveSpeed * 0.032f; // Assuming ~60 FPS
        sprite.position.x += directionX * stepSize;
        sprite.position.y += directionY * stepSize;

        // Update animation frame based on remaining distance
        if (defenseType == 2) // Upward jump (single frame)
        {
            sprite.updateFrame(1); // Upward jump frame
        }
        else // Left or Right Bottom
        {
            if (distance < totalMoveDistance / 3)
            {
                sprite.updateFrame(4); // Final frame for bottom defense
            }
            else if (distance < totalMoveDistance / 3 * 2)
            {
                sprite.updateFrame(3); // Midway frame for bottom defense
            }
            else
            {
                sprite.updateFrame(2); // Initial movement frame for bottom defense
            }
        }
    }
    else
    {
        // Reached target position, reset to idle and mark complete
        sprite.position = glm::vec3(sprite.position.x, idleYPosition, sprite.position.z); // Reset to fixed Y
        sprite.updateFrame(0); // Idle frame
        isGoalkeeperAnimationComplete = true;
        totalMoveDistance = 0.0f; // Reset for the next move
    }
}


void Goalkeeper::resetPositions()
{
    sprite.position = glm::vec3(400.0, 475.0, 0.0);
    sprite.updateFrame(0);
    totalMoveDistance = 0.0f;
    currentFrameIndex = 0;
}

#endif
