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

#include <random>

using namespace glm;

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

// Sprites
Sprite background, ball, player, goalkeeper, horizontalArrow, verticalArrow, redCircle;

// Controle para a seta de direção do chute
float arrowPosX = (goalLimits.leftBottom.x + goalLimits.rightTop.x) / 2.0f;
float arrowPosY = (goalLimits.leftBottom.y + goalLimits.rightTop.y) / 2.0f;
float arrowSpeed = 7.5f;
float ballSpeed = 75.0f;
float totalBallDistance = 0.0f;
bool isArrowMovingRight = true;
bool isArrowMovingUp = true;
bool isVerticalArrowMoving = false;
bool isPlayerShooting = true;
bool isPlayerSelectingTarget = true;
bool isKickAnimationComplete = false;
bool isBallAnimationComplete = false;
bool wasSpacePressed = false;

Coordinates kickTarget = {0.0f, 0.0f};

bool showRedCircle = false;
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

void setupBallSprite()
{
	int imgWidth, imgHeight = 0;
	int ballTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/ball/movement.png", imgWidth, imgHeight);
	ball.setupSprite(ballTexture, vec3(400.0, 200.0, 0.0), vec3(imgWidth / 3.0 * 3.0, imgHeight * 2, 1.5), 3, 1);
}

void setupPlayerSprite()
{
	int imgWidth, imgHeight = 0;
	int playerTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/player/movement.png", imgWidth, imgHeight);
	player.setupSprite(playerTexture, vec3(300.0, 175.0, 0.0), vec3(imgWidth / 2, imgHeight * 2.0, 1.0), 6, 1);
}

void setupGoalkeeperSprite()
{
	int imgWidth, imgHeight = 0;
	int idleGoalkeeperTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/goalkeeper/idle.png", imgWidth, imgHeight);
	goalkeeper.setupSprite(idleGoalkeeperTexture, vec3(400.0, 475.0, 0.0), vec3(imgWidth * 2, imgHeight * 3, 1.0), 1, 1);
}

void setupBackgroundSprite()
{
	int imgWidth, imgHeight = 0;
	int bgTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/background/movement.png", imgWidth, imgHeight);
	background.setupSprite(bgTexture, vec3(400.0, 300.0, 0.0), vec3(800.0, 600.0, 1.0), 2, 1);
}

void setupHorizontalArrowSprite()
{
	int imgWidth, imgHeight = 0;
	int arrowTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/arrow/arrow.png", imgWidth, imgHeight);
	horizontalArrow.setupSprite(arrowTexture, vec3(100.0, 300.0, 0.0), vec3(imgWidth * 2, imgHeight * 2, 1.0), 1, 1);
}

void setupVerticalArrowSprite()
{
	int imgWidth, imgHeight = 0;
	verticalArrow.angle = -90.0f;
	int arrowTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/arrow/arrow.png", imgWidth, imgHeight);
	verticalArrow.setupSprite(arrowTexture, vec3(175.0, 175.0, 0.0), vec3(imgWidth * 2, imgHeight * 2, 1.0), 1, 1);
}

void setupRedCircleSprite()
{
	int imgWidth, imgHeight = 0;
	int redCircleTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/circle/circle.png", imgWidth, imgHeight);
	redCircle.setupSprite(redCircleTexture, vec3(600.0, 0.0, 0.0), vec3(imgWidth * 5.0f, imgHeight * 5.0f, 1.0), 1, 1);
}

float randomFloat(float min, float max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(min, max);
	return dis(gen);
}

// Function to select a random position inside the goal limits
void selectRandomKickTarget()
{
	float randomX = randomFloat(goalLimits.leftBottom.x, goalLimits.rightTop.x);
	float randomY = randomFloat(goalLimits.leftBottom.y, goalLimits.rightTop.y);
	kickTarget = {randomX, randomY};
	redCircle.position = vec3(randomX, randomY, 0.0f);
}

void moveHorizontalArrow()
{
	// Movimenta a seta da esquerda para a direita
	if (isArrowMovingRight)
		arrowPosX += arrowSpeed;
	else
		arrowPosX -= arrowSpeed;
	// Inverta a direção se atingir os limites
	if (arrowPosX > goalLimits.rightTop.x * 1.1f)
		isArrowMovingRight = false;
	if (arrowPosX < goalLimits.leftBottom.x * 0.9f)
		isArrowMovingRight = true;
	// Atualiza a posição da seta
	horizontalArrow.position.x = arrowPosX;
}

void moveVerticalArrow()
{
	// Movimenta a seta de cima para baixo
	if (isArrowMovingUp)
		arrowPosY += arrowSpeed;
	else
		arrowPosY -= arrowSpeed;
	// Inverta a direção se atingir os limites
	if (arrowPosY > goalLimits.rightTop.y * 1.1f)
		isArrowMovingUp = false;
	if (arrowPosY < goalLimits.leftBottom.y * 0.9f)
		isArrowMovingUp = true;
	// Atualiza a posição da seta
	verticalArrow.position.y = arrowPosY;
}

void moveBall()
{
	// Calculate the total distance between the ball's current position and the target
	float dx = kickTarget.x - ball.position.x;
	float dy = kickTarget.y - ball.position.y;
	float distance = sqrt(dx * dx + dy * dy); // Hypotenuse

	// Only move the ball if the distance is greater than a small threshold
	if (distance > 1.0f)
	{
		// Normalize the direction vector by dividing by the distance (hypotenuse)
		float directionX = dx / distance;
		float directionY = dy / distance;

		// Move the ball by a fixed step size
		float stepSize = ballSpeed * 0.032f; // Assuming a constant frame time of 16ms (~60 FPS)
		ball.position.x += directionX * stepSize;
		ball.position.y += directionY * stepSize;

		// Update the animation frame based on the distance traveled
		if (distance < totalBallDistance / 3)
		{
			ball.iFrame = 2;
		}
		else if (distance < totalBallDistance / 3 * 2)
		{
			ball.iFrame = 1;
		}
		else
		{
			ball.iFrame = 0;
		}
		ball.updateFrame(); // Update the texture coordinates for the new frame
	}
	else
	{
		// Ensure the ball reaches the exact target when it's close enough
		ball.position.x = kickTarget.x;
		ball.position.y = kickTarget.y;
		isBallAnimationComplete = true;
	}
}

void movePlayer()
{

	float currentTime = glfwGetTime();
	float deltaTime = currentTime - player.lastTime;

	if (deltaTime >= 1.5 / player.FPS)
	{
		if (player.iFrame == 4)
		{
			player.iFrame = 5;
			player.position.x = 400.0f;
			player.position.y = 220.0f;
		}
		else if (player.iFrame == 3)
		{
			player.iFrame = 4;
			player.position.x = 380.0f;
			player.position.y = 210.0f;
		}
		else if (player.iFrame == 2)
		{
			player.iFrame = 3;
			player.position.x = 360.0f;
			player.position.y = 190.0f;
		}
		else if (player.iFrame == 1)
		{
			player.iFrame = 2;
			player.position.x = 340.0f;
			player.position.y = 185.0f;
		}
		else if (player.iFrame == 0)
		{
			player.iFrame = 1;
			player.position.x = 320.0f;
			player.position.y = 180.0f;
		}
		player.updateFrame();
		player.lastTime = glfwGetTime();
	}
	if (player.iFrame == 5)
	{
		isKickAnimationComplete = true;
	}
}

void resetPositions()
{
	ball.position = vec3(400.0, 200.0, 0.0);
	ball.iFrame = 0;
	ball.updateFrame();
	player.position = vec3(300.0, 175.0, 0.0);
	player.iFrame = 0;
	player.updateFrame();
	goalkeeper.position = vec3(400.0, 475.0, 0.0);
	goalkeeper.iFrame = 0;
	goalkeeper.updateFrame();
	isPlayerShooting = false;
	isPlayerSelectingTarget = false;
	isVerticalArrowMoving = false;
	isKickAnimationComplete = false;
	isBallAnimationComplete = false;
	showRedCircle = false;
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

	setupBackgroundSprite();
	setupBallSprite();
	setupPlayerSprite();
	setupGoalkeeperSprite();
	setupHorizontalArrowSprite();
	setupVerticalArrowSprite();
	setupRedCircleSprite();

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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawSprite(background, shaderID);
		drawSprite(goalkeeper, shaderID);
		drawSprite(ball, shaderID);
		drawSprite(player, shaderID);

		if (isPlayerShooting)
		{
			if (isPlayerSelectingTarget)
			{

				if (!isVerticalArrowMoving)
				{
					moveHorizontalArrow();
					drawSprite(horizontalArrow, shaderID);
				}
				else
				{
					moveVerticalArrow();
					drawSprite(verticalArrow, shaderID);
					drawSprite(horizontalArrow, shaderID);
				}

				// Debounce the spacebar key press
				if (keys[GLFW_KEY_SPACE] && !wasSpacePressed)
				{
					if (!isVerticalArrowMoving)
					{
						kickTarget.x = arrowPosX;
						isVerticalArrowMoving = true;
					}
					else
					{
						kickTarget.y = arrowPosY;
						ball.lastTime = glfwGetTime();
						isPlayerSelectingTarget = false;
						totalBallDistance = sqrt(pow(kickTarget.x - ball.position.x, 2) + pow(kickTarget.y - ball.position.y, 2));
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
				if (!isKickAnimationComplete)
				{
					movePlayer();
				}
				else
				{
					if (!isBallAnimationComplete)
					{
						moveBall();
					}
					else
					{
						resetPositions();
						isPlayerShooting = false;
					}
				}
			}
		}
		else
		{
			if (!showRedCircle)
			{
				selectRandomKickTarget();
				showRedCircle = true;
			}

			// Mostra o círculo vermelho onde a bola irá
			if (showRedCircle)
			{
				circleTimer += glfwGetTime();
				if (circleTimer < circleDisplayTime)
				{
					isKickAnimationComplete = false;
					drawSprite(redCircle, shaderID);
				}
				else
				{
					if (!isKickAnimationComplete)
					{
						movePlayer();
					}
					else
					{
						if (!isBallAnimationComplete)
						{
							moveBall();
						}
						else
						{
							resetPositions();
							isPlayerShooting = true;
							isPlayerSelectingTarget = true;
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
