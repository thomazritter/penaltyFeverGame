#ifndef TARGET_H
#define TARGET_H

#include "sprite.h"
#include <GLFW/glfw3.h>
#include <utils.h>

class Target
{
public:
	Sprite sprite;

	Target();
	void setupSprite();
	void selectRandomKickTarget(Coordinates &kickTarget, GoalLimits goalLimits);
};

Target::Target() {}

void Target::setupSprite()
{
	int imgWidth, imgHeight = 0;
	int redCircleTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/circle/circle.png", imgWidth, imgHeight);
	sprite.setupSprite(redCircleTexture, glm::vec3(600.0, 0.0, 0.0), glm::vec3(imgWidth * 5.0f, imgHeight * 5.0f, 1.0), 1, 1);
}

void Target::selectRandomKickTarget(Coordinates &kickTarget, GoalLimits goalLimits)
{
	float randomX = randomFloat(goalLimits.leftBottom.x, goalLimits.rightTop.x);
	float randomY = randomFloat(goalLimits.leftBottom.y, goalLimits.rightTop.y);
	kickTarget = {randomX, randomY};
	sprite.position = glm::vec3(randomX, randomY, 0.0f);
}

#endif // TARGET_H