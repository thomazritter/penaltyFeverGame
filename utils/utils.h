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

float randomFloat(float min, float max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

Coordinates determineTargetSection(GoalSection section, GoalLimits goalLimits)
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
        target.x = 1000.0f;
        target.y = 1000.0f;
        break;
    }
    return target;
}

#endif