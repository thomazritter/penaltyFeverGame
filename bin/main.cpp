#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STB_IMAGE
#include <stb_image.h>

#include <sprite.h>
#include <Shader.h>

#include <utils.h>

#include <player.h>
#include <ball.h>
#include <arrow.h>
#include <target.h>
#include <background.h>
#include <goalkeeper.h>

using namespace glm;

GoalLimits goalLimits = {
	{209.0f, 414.0f}, // leftBottom
	{591.0f, 550.0f}  // rightTop
};

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

float vel = 1.2;

bool keys[1024] = {false};

// Shader
GLuint shaderID;

Player player;
Ball ball;
Arrow horizontalArrow = Arrow(true);
Arrow verticalArrow = Arrow(false);
Target target;
Background background;
Goalkeeper goalkeeper;

// Controle para a seta de direção do chute
bool isVerticalArrowMoving = false;
bool isPlayerShooting = true;
bool isPlayerSelectingTarget = true;
bool isKickAnimationComplete = false;
bool isBallAnimationComplete = false;
bool wasSpacePressed = false;
bool isGoalkeeperAnimationComplete = false;

Coordinates kickTarget = {0.0f, 0.0f};

bool showTarget = false;
float circleDisplayTime = 360.0f;
float circleTimer = 0.0f;

// Resultado do chute
bool playerScored = false;
bool playerDefended = false;

bool mouseClickedOn(GLFWwindow *window, vec3 targetPos)
{
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	float dx = mouseX - targetPos.x;
	float dy = HEIGHT - mouseY - targetPos.y;

	return sqrt(dx * dx + dy * dy) < 50.0f; // Raio de tolerância para defesa
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Get the window height to invert the y-coordinate
		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		ypos = windowHeight - ypos;

		std::cout << "Mouse clicked at: (" << xpos << ", " << ypos << ")" << std::endl;
	}
}

// Function to set up the mouse click callback
void setupMouseClickLogging(GLFWwindow *window)
{
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;

	if (action == GLFW_RELEASE)
		keys[key] = false;
}

void resetPositions()
{
	ball.resetPositions();
	player.resetPositions();
	goalkeeper.resetPositions();
	isPlayerShooting = false;
	isPlayerSelectingTarget = false;
	isVerticalArrowMoving = false;
	isKickAnimationComplete = false;
	isBallAnimationComplete = false;
	showTarget = false;
	circleTimer = 0.0f;
}

int main()
{
	glfwInit();
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Penalty Fever", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	// Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte *version = glGetString(GL_VERSION);	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	// Compilando e buildando o programa de shader
	// Gerando um buffer simples, com a geometria de um triângulo

	background.setupSprite();
	player.setupSprite();
	ball.setupSprite();
	goalkeeper.setupSprite();
	horizontalArrow.setupSprite();
	verticalArrow.setupSprite();
	target.setupSprite();

	shaderID = setupShader();
	glUseProgram(shaderID);
	glUniform1i(glGetUniformLocation(shaderID, "texBuffer"), 0);
	mat4 projection = ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	// Set up mouse click logging
	setupMouseClickLogging(window);
	glDepthFunc(GL_ALWAYS);

	while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSprite(background.sprite, shaderID);
        drawSprite(goalkeeper.sprite, shaderID);
        drawSprite(ball.sprite, shaderID);
        drawSprite(player.sprite, shaderID);

        if (isPlayerShooting)
        {
            if (isPlayerSelectingTarget)
            {
                if (!isVerticalArrowMoving)
                {
                    horizontalArrow.move(goalLimits);
                    drawSprite(horizontalArrow.sprite, shaderID);
                }
                else
                {
                    verticalArrow.move(goalLimits);
                    drawSprite(verticalArrow.sprite, shaderID);
                    drawSprite(horizontalArrow.sprite, shaderID);
                }

                if (keys[GLFW_KEY_SPACE] && !wasSpacePressed)
                {
                    if (!isVerticalArrowMoving)
                    {
                        kickTarget.x = horizontalArrow.sprite.position.x;
                        isVerticalArrowMoving = true;
                    }
                    else
                    {
                        kickTarget.y = verticalArrow.sprite.position.y;
                        ball.sprite.lastTime = glfwGetTime();
                        isPlayerSelectingTarget = false;
                        ball.totalBallDistance = sqrt(pow(kickTarget.x - ball.sprite.position.x, 2) +
                                                      pow(kickTarget.y - ball.sprite.position.y, 2));
                    }
                    wasSpacePressed = true;
                }
                else if (!keys[GLFW_KEY_SPACE])
                {
                    wasSpacePressed = false;
                }
            }
            else
            {
                if (!showTarget)
                {
                    target.selectExactKick(kickTarget, goalLimits);
                    showTarget = true;
                }

                if (showTarget)
                {
                    circleTimer += glfwGetTime();
                    if (circleTimer < circleDisplayTime)
                    {
                        isKickAnimationComplete = false;
                        drawSprite(target.sprite, shaderID);
                    }
                    else
                    {
                        if (!isKickAnimationComplete)
                        {
                            player.movePlayer(isKickAnimationComplete);
                        }
                        else
                        {
                            if (!isBallAnimationComplete)
                            {
                                ball.moveBall(isBallAnimationComplete, kickTarget);
                            }
                            else
                            {
                                // Animate goalkeeper's defense attempt
                                goalkeeper.moveGoalkeeper(isGoalkeeperAnimationComplete, goalLimits);

                                // If the goalkeeper's animation is complete, reset positions for the next shot
                                if (isGoalkeeperAnimationComplete)
                                {
                                    resetPositions();
                                    isPlayerShooting = true;
                                    isPlayerSelectingTarget = true;
                                    isGoalkeeperAnimationComplete = false; // Reset flag
                                }
                            }
                        }
                    }
                }
            }
        }
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}