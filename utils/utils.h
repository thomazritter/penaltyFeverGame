#ifndef UTILS_H
#define UTILS_H

#include <random>

struct Coordinates
{
    float x;
    float y;
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

#endif