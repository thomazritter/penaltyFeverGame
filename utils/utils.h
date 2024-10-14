#ifndef UTILS_H
#define UTILS_H

#include <random>

struct Coordinates
{
    float x;
    float y;
};

enum GoalSection
{
    LEFT_BOTTOM,
    LEFT_TOP,
    MIDDLE_BOTTOM,
    MIDDLE_TOP,
    RIGHT_BOTTOM,
    RIGHT_TOP,
    OUTSIDE
};
struct GoalLimits
{
    Coordinates leftBottom;
    Coordinates rightTop;
};

extern const GoalLimits goalLimits = {
    {209.0f, 414.0f}, // leftBottom
    {591.0f, 550.0f}  // rightTop
};

float randomFloat(float min, float max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

GoalSection determineGoalSection(Coordinates target)
{
    if (target.x < goalLimits.leftBottom.x || target.x > goalLimits.rightTop.x ||
        target.y < goalLimits.leftBottom.y || target.y > goalLimits.rightTop.y)
    {
        return OUTSIDE;
    }

    float thirdX = (goalLimits.rightTop.x - goalLimits.leftBottom.x) / 3.0f;
    float midY = (goalLimits.leftBottom.y + goalLimits.rightTop.y) / 2.0f;

    if (target.x < goalLimits.leftBottom.x + thirdX)
    {
        if (target.y < midY)
        {
            return LEFT_BOTTOM;
        }
        else
        {
            return LEFT_TOP;
        }
    }
    else if (target.x < goalLimits.leftBottom.x + 2 * thirdX)
    {
        if (target.y < midY)
        {
            return MIDDLE_BOTTOM;
        }
        else
        {
            return MIDDLE_TOP;
        }
    }
    else
    {
        if (target.y < midY)
        {
            return RIGHT_BOTTOM;
        }
        else
        {
            return RIGHT_TOP;
        }
    }
}

Coordinates determineTargetCoordinates(GoalSection section)
{
    Coordinates target;
    float thirdX = (goalLimits.rightTop.x - goalLimits.leftBottom.x) / 3.0f;
    float midY = (goalLimits.leftBottom.y + goalLimits.rightTop.y) / 2.0f;
    switch (section)
    {
    case LEFT_BOTTOM:
        target.x = goalLimits.leftBottom.x + thirdX / 2.0f;
        target.y = goalLimits.leftBottom.y + (midY - goalLimits.leftBottom.y) / 2.0f;
        break;
    case LEFT_TOP:
        target.x = goalLimits.leftBottom.x + thirdX / 2.0f;
        target.y = goalLimits.rightTop.y - (goalLimits.rightTop.y - midY) / 2.0f;
        break;
    case MIDDLE_BOTTOM:
        target.x = goalLimits.leftBottom.x + thirdX + thirdX / 2.0f;
        target.y = goalLimits.leftBottom.y + (midY - goalLimits.leftBottom.y) / 2.0f;
        break;
    case MIDDLE_TOP:
        target.x = goalLimits.leftBottom.x + thirdX + thirdX / 2.0f;
        target.y = goalLimits.rightTop.y - (goalLimits.rightTop.y - midY) / 2.0f;
        break;
    case RIGHT_BOTTOM:
        target.x = goalLimits.leftBottom.x + 2 * thirdX + thirdX / 2.0f;
        target.y = goalLimits.leftBottom.y + (midY - goalLimits.leftBottom.y) / 2.0f;
        break;
    case RIGHT_TOP:
        target.x = goalLimits.leftBottom.x + 2 * thirdX + thirdX / 2.0f;
        target.y = goalLimits.rightTop.y - (goalLimits.rightTop.y - midY) / 2.0f;
        break;
    default:
        target.x = 800.0f;
        target.y = 600.0f;
        break;
    }
    return target;
}

Coordinates selectRandomTargetInsideGoal()
{
    float randomX = randomFloat(goalLimits.leftBottom.x, goalLimits.rightTop.x);
    float randomY = randomFloat(goalLimits.leftBottom.y, goalLimits.rightTop.y);
    Coordinates target = {randomX, randomY};
    return target;
}

Coordinates selectRandomTargetAnywhere()
{
    float randomX = randomFloat(goalLimits.leftBottom.x * 1.025, goalLimits.rightTop.x * 1.025);
    float randomY = randomFloat(goalLimits.leftBottom.y * 1.025, goalLimits.rightTop.y * 1.025);
    Coordinates target = {randomX, randomY};
    return target;
}

#endif