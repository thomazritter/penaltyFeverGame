#ifndef UTILS_H
#define UTILS_H

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

#endif