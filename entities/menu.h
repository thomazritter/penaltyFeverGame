#ifndef MENU_H
#define MENU_H

#include "sprite.h"
#include <GLFW/glfw3.h>

class Menu
{
public:
    Sprite menu;
    Sprite button;

    Menu();
    void setupSprite();
};

Menu::Menu() {}

void Menu::setupSprite()
{
    int imgWidth, imgHeight = 0;
    int menuTexture = loadTexture("/sprites/menu/menu.png", imgWidth, imgHeight);
    menu.setupSprite(menuTexture, glm::vec3(400.0, 300.0, 0.0), glm::vec3(800.0, 600.0, 1.0), 1, 1);

    imgWidth, imgHeight = 0;
    int buttonTexture = loadTexture("/sprites/menu/button.png", imgWidth, imgHeight);
    button.setupSprite(buttonTexture, glm::vec3(400.0, 50.0, 0.0), glm::vec3(200.0, 75.0, 1.0), 1, 1);
}

#endif
