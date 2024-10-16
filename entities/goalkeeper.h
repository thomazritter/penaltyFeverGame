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
    Coordinates defenseTarget;

    Goalkeeper();
    void setupSprite();
    void moveGoalkeeper(bool &isGoalkeeperAnimationComplete);
    void resetPositions();
    void defineTargetBySection(GoalSection section);

private:
    vector<Coordinates> jumpParabola;
    void calculateJumpParabola(Coordinates jumpTarget);
    int parabolaPosition = 0;
    Coordinates startCoordinates = {400.0, 475.0};
};

Goalkeeper::Goalkeeper() {}

void Goalkeeper::setupSprite()
{
    int imgWidth, imgHeight = 0;

    // Load left-side texture (movement.png)
    int goalkeeperTexture = loadTexture("/sprites/goalkeeper/movement.png", imgWidth, imgHeight);
    sprite.setupSprite(goalkeeperTexture, glm::vec3(400.0, 475.0, 0.0), glm::vec3(imgWidth / 4.0, imgHeight * 3.0, 1.0), 7, 1);
}

void Goalkeeper::moveGoalkeeper(bool &isGoalkeeperAnimationComplete)
{
    GoalSection defenseSection = determineGoalSection(defenseTarget);
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - sprite.lastTime;
    if (defenseSection == MIDDLE_BOTTOM)
    {
        sprite.position.x = defenseTarget.x;
        sprite.position.y = defenseTarget.y;
        isGoalkeeperAnimationComplete = true;
    }
    else if (defenseSection == MIDDLE_TOP)
    {
        sprite.updateFrame(1);
        isGoalkeeperAnimationComplete = true;
    }
    else
    {
        sprite.isMirrored = defenseSection == LEFT_BOTTOM || defenseSection == LEFT_TOP ? false : true;
        if (deltaTime >= 1.0f / sprite.FPS)
        {
            if (parabolaPosition < jumpParabola.size())
            {
                if (parabolaPosition > 4 * jumpParabola.size() / 5)
                {
                    sprite.updateFrame(6);
                }
                else if (parabolaPosition > 3 * jumpParabola.size() / 5)
                {
                    sprite.updateFrame(5);
                }
                else if (parabolaPosition > 2 * jumpParabola.size() / 5)
                {
                    sprite.updateFrame(4);
                }
                else if (parabolaPosition > jumpParabola.size() / 5)
                {
                    sprite.updateFrame(3);
                }
                else
                {
                    sprite.updateFrame(2);
                }

                sprite.position.x = jumpParabola[parabolaPosition].x;
                sprite.position.y = jumpParabola[parabolaPosition].y;
                parabolaPosition++;
            }
            else
            {
                isGoalkeeperAnimationComplete = true;
            }
            sprite.lastTime = currentTime;
        }
    }
}

void Goalkeeper::resetPositions()
{
    sprite.position = glm::vec3(400.0, 475.0, 0.0);
    jumpParabola.clear();
    parabolaPosition = 0;
    sprite.updateFrame(0); // Idle frame for left-side texture (movement.png)
}

void Goalkeeper::defineTargetBySection(GoalSection section)
{
    Coordinates finalTarget = determineTargetCoordinates(section);

    defenseTarget = finalTarget;

    if (section != MIDDLE_BOTTOM && section != MIDDLE_TOP)
    {
        calculateJumpParabola(finalTarget);
    }
}

void Goalkeeper::calculateJumpParabola(Coordinates jumpTarget)
{
    // Calculate the distance between startCoordinates and the roots
    float distance = jumpTarget.x - startCoordinates.x;
    float root1 = startCoordinates.x;
    float root2 = jumpTarget.x + distance;

    // A negative value for 'a' makes the parabola open downwards
    float a = -0.0001;

    // Calculate parabola points
    int steps = sprite.FPS; // Number of steps for smoothness
    float increment = distance / steps;

    if (jumpTarget.x > startCoordinates.x)
    {
        for (float x = root1; x <= root2; x += increment)
        {
            // Use the vertex form of the parabola
            float y = a * (x - root1) * (x - root2) + startCoordinates.y;
            jumpParabola.push_back({x, y});
        }
    }
    else
    {
        // The jump can also go in the negative direction (i.e., x < startCoordinates.x)
        for (float x = root1; x >= root2; x += increment)
        {
            float y = a * (x - root1) * (x - root2) + startCoordinates.y;
            jumpParabola.push_back({x, y});
        }
    }
}

#endif
