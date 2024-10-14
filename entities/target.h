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
	void setTarget(Coordinates kickTarget);
	void setTargetBySection(GoalSection section);
};

Target::Target() {}

void Target::setupSprite()
{
	int imgWidth, imgHeight = 0;
	int redCircleTexture = loadTexture("/sprites/circle/circle.png", imgWidth, imgHeight);
	sprite.setupSprite(redCircleTexture, glm::vec3(600.0, 0.0, 0.0), glm::vec3(imgWidth * 5.0f, imgHeight * 5.0f, 1.0), 1, 1);
}

void Target::setTarget(Coordinates kickTarget)
{
	sprite.position = glm::vec3(kickTarget.x, kickTarget.y, 0.0f);
}

void Target::setTargetBySection(GoalSection section)
{
	Coordinates target = determineTargetCoordinates(section);
	sprite.position = glm::vec3(target.x, target.y, 0.0f);
}

#endif