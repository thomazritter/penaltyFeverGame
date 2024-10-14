#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "sprite.h"
#include <GLFW/glfw3.h>

class Scoreboard
{
public:
    Sprite playerScoreboardSprite;
    Sprite opponentScoreboardSprite;

    Scoreboard();
    void setupSprite();
    void setupPlayerScore(int score);
    void setupOpponentScore(int score);
};

Scoreboard::Scoreboard() {}

void Scoreboard::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int scoreboardTexture = loadTexture("/sprites/scoreboard/scoreboard.png", imgWidth, imgHeight);
    playerScoreboardSprite.setupSprite(scoreboardTexture, glm::vec3(610.0, 150.0, 0.0), glm::vec3(imgWidth / 2, imgHeight * 5, 1.0), 10, 1);
    opponentScoreboardSprite.setupSprite(scoreboardTexture, glm::vec3(710.0, 150.0, 0.0), glm::vec3(imgWidth / 2, imgHeight * 5, 1.0), 10, 1);
}

void Scoreboard::setupPlayerScore(int score)
{
    playerScoreboardSprite.updateFrame(score);
}

void Scoreboard::setupOpponentScore(int score)
{
    opponentScoreboardSprite.updateFrame(score);
}

#endif
