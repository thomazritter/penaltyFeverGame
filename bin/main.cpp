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
void renderBallMovement(Sprite &ball, GLuint shaderID, float ballTargetPosX, float ballTargetPosY, float ballSpeed, float totalDistance);

// Protótipos das funções
int loadTexture(string filePath, int &imgWidth, int &imgHeight);
void drawSprite(Sprite spr, GLuint shaderID);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

float vel = 1.2;

bool keys[1024] = {false};

// Controle para a seta de direção do chute
float arrowPosX = 0.0f;
float arrowPosY = 487.5f;
float arrowSpeed = 7.5f;
float ballSpeed = 0.5f;
float totalDistance = 0.0f;
bool isArrowMovingRight = true;
bool isArrowMovingUp = true;
bool isVerticalArrowMoving = false;
bool isPlayerShooting = true;
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
	GLuint shaderID = setupShader();
	// Gerando um buffer simples, com a geometria de um triângulo
	// Sprite do fundo da cena
	Sprite background, ball, player, goalkeeper, horizontalArrow, verticalArrow, redCircle;
	// Carregando uma textura (recebendo seu ID)
	// Inicializando a sprite do background
	int imgWidth, imgHeight;
	// Carregar texturas
	int bgTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/background/background.png", imgWidth, imgHeight);
	int idlePlayer = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/player/player-idle.png", imgWidth, imgHeight);
	int arrowTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/arrow/arrow.png", imgWidth, imgHeight);
	int redCircleTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/circle/circle.png", imgWidth, imgHeight);
	int idleGoalkeeperTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/goalkeeper/idle.png", imgWidth, imgHeight);
	int movingPlayer = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/player/movement.png", imgWidth, imgHeight);
	int ballTexture = loadTexture("C:/Users/Carlos/Desktop/Unisinos/7semestre/PG/AtividadesPG/penaltyFeverGame/sprites/ball/movement.png", imgWidth, imgHeight);
	ball.setupSprite(ballTexture, vec3(400.0, 200.0, 0.0), vec3(imgWidth / 3.0 * 2.0, imgHeight * 2, 1.0), 3, 1);
	background.setupSprite(bgTexture, vec3(400.0, 300.0, 0.0), vec3(800.0, 600.0, 1.0), 1, 1);
	goalkeeper.setupSprite(idleGoalkeeperTexture, vec3(400.0, 475.0, 0.0), vec3(100.0, 150.0, 1.0), 1, 1);
	player.setupSprite(idlePlayer, vec3(300.0, 175.0, 0.0), vec3(100.0, 150.0, 1.0), 1, 1);
	horizontalArrow.setupSprite(arrowTexture, vec3(100.0, 300.0, 0.0), vec3(50.0, 50.0, 1.0), 1, 1);
	verticalArrow.angle = -90.0f;
	verticalArrow.setupSprite(arrowTexture, vec3(175.0, 175.0, 0.0), vec3(50.0, 50.0, 1.0), 1, 1);
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
		vec2 offsetTex = vec2(0.0, 0.0);
		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);
		// Desenhar o background
		drawSprite(background, shaderID);

		drawSprite(goalkeeper, shaderID);
		drawSprite(player, shaderID);
		// Modo de chute do jogador
		if (isPlayerShooting)
		{
			if (isPlayerSelectingTarget)
			{
				drawSprite(ball, shaderID);
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
						totalDistance = sqrt(pow(ballTargetPosX - ball.position.x, 2) + pow(ballTargetPosY - ball.position.y, 2));
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
				renderBallMovement(ball, shaderID, ballTargetPosX, ballTargetPosY, ballSpeed, totalDistance);
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

void renderBallMovement(Sprite &ball, GLuint shaderID, float ballTargetPosX, float ballTargetPosY, float ballSpeed, float totalDistance)
{
	float currentTime = glfwGetTime();
	float deltaTime = currentTime - ball.lastTime;
	float currentDistance = sqrt(pow(ballTargetPosX - ball.position.x, 2) + pow(ballTargetPosY - ball.position.y, 2));

	vec2 offsetTex = vec2(0.0, 0.0);
	glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);

	if (deltaTime >= 1.0 / ball.FPS)
	{
		if (ball.iFrame < 2 && currentDistance < totalDistance / 3)
		{
			ball.iFrame = 2;
		}
		else if (ball.iFrame < 1 && currentDistance < totalDistance / 3 * 2)
		{
			ball.iFrame = 1;
		}
		ball.lastTime = glfwGetTime();
	}
	offsetTex.s = ball.iFrame * ball.d.s;
	offsetTex.t = 0.0;
	glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);
	ball.position = glm::mix(ball.position, vec3(ballTargetPosX, ballTargetPosY, 0.0f), deltaTime * ballSpeed);
	drawSprite(ball, shaderID);
}