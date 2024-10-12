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

using namespace glm;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
int loadTexture(string filePath, int &imgWidth, int &imgHeight);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

float vel = 1.2;

bool keys[1024] = {false};

// Shader
GLuint shaderID;

// Sprites
Sprite background, ball, player, goalkeeper, horizontalArrow, verticalArrow, redCircle;

// Controle para a seta de direção do chute
float arrowPosX = 0.0f;
float arrowPosY = 487.5f;
float arrowSpeed = 7.5f;
float ballSpeed = 0.5f;
float totalBallDistance = 0.0f;
bool isArrowMovingRight = true;
bool isArrowMovingUp = true;
bool isVerticalArrowMoving = false;
bool isPlayerShooting = true;
bool isKickAnimationComplete = false;
bool isPlayerGoalkeeper = false;
bool isPlayerSelectingTarget = true;
bool wasSpacePressed = false;

float ballTargetPosX = 0.0f;
float ballTargetPosY = 0.0f;

bool showRedCircle = false;
float circleDisplayTime = 1.0f;
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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;

	if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// Verifica onde o jogador clicou durante a defesa
	}
}

void setupBallSprite()
{
	int imgWidth, imgHeight = 0;
	int ballTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/ball/movement.png", imgWidth, imgHeight);
	ball.setupSprite(ballTexture, vec3(400.0, 200.0, 0.0), vec3(imgWidth / 3.0 * 2.0, imgHeight * 2, 1.0), 3, 1);
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
	int bgTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/background/background.png", imgWidth, imgHeight);
	background.setupSprite(bgTexture, vec3(400.0, 300.0, 0.0), vec3(800.0, 600.0, 1.0), 1, 1);
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
	redCircle.setupSprite(redCircleTexture, vec3(400.0, 475.0, 0.0), vec3(100.0, 150.0, 1.0), 1, 1);
}

void moveHorizontalArrow()
{
	// Movimenta a seta da esquerda para a direita
	if (isArrowMovingRight)
		arrowPosX += arrowSpeed;
	else
		arrowPosX -= arrowSpeed;
	// Inverta a direção se atingir os limites
	if (arrowPosX > 625.0f)
		isArrowMovingRight = false;
	if (arrowPosX < 175.0f)
		isArrowMovingRight = true;
	// Atualiza a posição da seta
	horizontalArrow.position.x = arrowPosX;
	// Mostra a seta de direção
	drawSprite(horizontalArrow, shaderID);
}

void moveVerticalArrow()
{
	// Movimenta a seta de cima para baixo
	if (isArrowMovingUp)
		arrowPosY += arrowSpeed;
	else
		arrowPosY -= arrowSpeed;
	// Inverta a direção se atingir os limites
	if (arrowPosY > 575.0f)
		isArrowMovingUp = false;
	if (arrowPosY < 400.0f)
		isArrowMovingUp = true;
	// Atualiza a posição da seta
	verticalArrow.position.y = arrowPosY;
	// Mostra as setas de direção
	drawSprite(verticalArrow, shaderID);
	drawSprite(horizontalArrow, shaderID);
}

void renderBallMovement()
{
	float currentTime = glfwGetTime();
	float deltaTime = currentTime - ball.lastTime;
	float currentDistance = sqrt(pow(ballTargetPosX - ball.position.x, 2) + pow(ballTargetPosY - ball.position.y, 2));

	if (deltaTime >= 1.0f / ball.FPS)
	{
		// Update frame and texture coordinates based on the current distance
		if (ball.iFrame < 2 && currentDistance < totalBallDistance / 3)
		{
			ball.iFrame = 2;
		}
		else if (ball.iFrame < 1 && currentDistance < totalBallDistance / 3 * 2)
		{
			ball.iFrame = 1;
		}
		ball.updateFrame(); // Update the texture coordinates
		ball.lastTime = currentTime;
	}
	ball.position = glm::mix(ball.position, vec3(ballTargetPosX, ballTargetPosY, 0.0f), deltaTime * ballSpeed);
}

void renderPlayerKick()
{

	float currentTime = glfwGetTime();
	float deltaTime = currentTime - player.lastTime;

	if (deltaTime >= 1.5 / player.FPS)
	{
		if (player.iFrame == 4)
		{
			player.iFrame = 5;
			player.position.x = 400.0f;
			player.position.y = 200.0f;
		}
		else if (player.iFrame == 3)
		{
			player.iFrame = 4;
			player.position.x = 380.0f;
			player.position.y = 195.0f;
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

	shaderID = setupShader();
	glUseProgram(shaderID);
	glUniform1i(glGetUniformLocation(shaderID, "texBuffer"), 0);
	mat4 projection = ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Desenhar o background
		drawSprite(background, shaderID);
		drawSprite(goalkeeper, shaderID);

		// Modo de chute do jogador
		if (isPlayerShooting)
		{
			drawSprite(ball, shaderID);
			drawSprite(player, shaderID);

			if (isPlayerSelectingTarget)
			{

				if (!isVerticalArrowMoving)
				{
					// Movimenta a seta da esquerda para a direita
					if (isArrowMovingRight)
						arrowPosX += arrowSpeed;
					else
						arrowPosX -= arrowSpeed;
					// Inverta a direção se atingir os limites
					if (arrowPosX > 625.0f)
						isArrowMovingRight = false;
					if (arrowPosX < 175.0f)
						isArrowMovingRight = true;
					// Atualiza a posição da seta
					horizontalArrow.position.x = arrowPosX;
					// Mostra a seta de direção
					drawSprite(horizontalArrow, shaderID);
				}
				else
				{
					// Movimenta a seta de cima para baixo
					if (isArrowMovingUp)
						arrowPosY += arrowSpeed;
					else
						arrowPosY -= arrowSpeed;
					// Inverta a direção se atingir os limites
					if (arrowPosY > 575.0f)
						isArrowMovingUp = false;
					if (arrowPosY < 400.0f)
						isArrowMovingUp = true;
					// Atualiza a posição da seta
					verticalArrow.position.y = arrowPosY;
					// Mostra as setas de direção
					drawSprite(verticalArrow, shaderID);
					drawSprite(horizontalArrow, shaderID);
				}

				// Debounce the spacebar key press
				if (keys[GLFW_KEY_SPACE] && !wasSpacePressed)
				{
					if (!isVerticalArrowMoving)
					{
						ballTargetPosX = arrowPosX;
						isVerticalArrowMoving = true;
					}
					else
					{
						ballTargetPosY = arrowPosY;
						ball.lastTime = glfwGetTime();
						isPlayerSelectingTarget = false;
						totalBallDistance = sqrt(pow(ballTargetPosX - ball.position.x, 2) + pow(ballTargetPosY - ball.position.y, 2));
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
					renderPlayerKick();
				}
				else
				{
					renderBallMovement();
				}
			}
		}
		// Modo de goleiro do jogador
		if (isPlayerGoalkeeper)
		{
			// Mostra o círculo vermelho onde a bola irá
			if (showRedCircle)
			{
				circleTimer += glfwGetTime();
				if (circleTimer < circleDisplayTime)
				{
					// redCircle.position = ballTargetPos;
					drawSprite(redCircle, shaderID);
				}
				else
				{
					showRedCircle = false;
					circleTimer = 0.0f;
				}
			}
			// // Se o jogador clicar no lugar certo, ele defende
			// if (mouseClickedOn(ballTargetPos))
			// {
			//  playerDefended = true;
			//  isPlayerGoalkeeper = false;
			// }
		}
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}
