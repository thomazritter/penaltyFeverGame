#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../shader/Shader.h"
#include "../sprites/sprite.h"
#include "../Dependencies/stb_image/stb_image.h"

// Configurações da janela
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Sprite &arrow);

bool movingRight = true; // Controla a direção do movimento
float arrowSpeed = 0.005f; // Velocidade de movimentação da seta
float targetDirection = 0.0f; // Valor inicial para a direção do chute
bool ballIsMoving = false;
float ballSpeed = 0.02f; // Velocidade da bola
float goalkeeperTargetPosition = 0.0f;  // Posição para onde o goleiro se moverá (esquerda, centro, ou direita)
float goalkeeperSpeed = 0.02f;           // Velocidade de movimento do goleiro
bool goalkeeperMoving = false;           // Controle para saber se o goleiro está se movendo
int playerScore = 0; // Pontuação do jogador
int goalkeeperScore = 0; // Pontuação do goleiro
int currentRound = 1; // Rodada atual
const int maxRounds = 5; // Número máximo de rodadas
bool playerTurn = true; // Controla de quem é a vez (jogador ou goleiro)

int main() {
    // Inicialização do GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criar janela
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Penalty Shooter Game", NULL, NULL);
    if (window == NULL) {
        std::cout << "Erro ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Erro ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Configurar viewport
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Ativar transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Carregar shaders
    Shader shader("tex.vs", "tex.fs");

    // Carregar textura do jogador (exemplo de arquivo player.png)
    GLuint playerTexture = loadTexture("sprites/player.png");
	GLuint fieldTexture = loadTexture("sprites/field.png");
	GLuint goalkeeperTexture = loadTexture("sprites/goalkeeper.png");
	GLuint ballTexture = loadTexture("sprites/ball.png");
	GLuint arrowTexture = loadTexture("sprites/arrow.png");

    // Inicializar o sprite do jogador
    Sprite player(playerTexture, glm::vec2(0.0f, -0.8f), glm::vec2(0.2f, 0.2f));
	Sprite field(fieldTexture, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	Sprite goalkeeper(goalkeeperTexture, glm::vec2(0.0f, 0.8f), glm::vec2(0.3f, 0.3f));
	Sprite ball(ballTexture, glm::vec2(0.0f, -0.3f), glm::vec2(0.05f, 0.05f));
	Sprite arrow(arrowTexture, glm::vec2(0.0f, -0.4f), glm::vec2(0.1f, 0.1f));

	// Loop principal do jogo
	while (!glfwWindowShouldClose(window) && currentRound <= maxRounds) {
		// Processar entrada do teclado
		processInput(window, arrow);

		// Atualizar posição da seta
		if (!ballIsMoving && playerTurn) {
			if (movingRight) {
				arrow.position.x += arrowSpeed;
				if (arrow.position.x > 0.4f) // Limite direito
					movingRight = false;
			} else {
				arrow.position.x -= arrowSpeed;
				if (arrow.position.x < -0.4f) // Limite esquerdo
					movingRight = true;
			}
		}

		// Atualizar posição da bola se estiver em movimento
		if (ballIsMoving) {
			// Mover a bola para cima (em direção ao gol)
			ball.position.y += ballSpeed;

			// Mover horizontalmente baseado na direção da seta
			if (targetDirection > 0) {
				ball.position.x += ballSpeed * (targetDirection / 0.4f);
			} else if (targetDirection < 0) {
				ball.position.x += ballSpeed * (targetDirection / 0.4f);
			}

			// Parar a bola quando ela chegar ao gol (defina um valor adequado)
			if (ball.position.y > 0.8f) {
				ballIsMoving = false;

				if (playerTurn) {
					// Verificar se o goleiro defendeu
					if (abs(ball.position.x - goalkeeper.position.x) < 0.1f) {
						std::cout << "Defesa do goleiro!" << std::endl;
					} else {
						std::cout << "Gol do jogador!" << std::endl;
						playerScore++;
					}
				} else {
					// Verificar se o goleiro marcou
					if (abs(ball.position.x - goalkeeper.position.x) > 0.1f) {
						std::cout << "Gol do goleiro!" << std::endl;
						goalkeeperScore++;
					} else {
						std::cout << "Jogador defendeu!" << std::endl;
					}
				}

				// Avançar para a próxima rodada
				if (playerTurn) {
					playerTurn = false; // Agora é a vez do goleiro
				} else {
					playerTurn = true; // Agora é a vez do jogador
					currentRound++; // Próxima rodada
				}

				// Resetar posição da bola
				ball.position = glm::vec2(0.0f, -0.3f);
			}
		}

		// Atualizar posição do goleiro se ele estiver se movendo
		if (goalkeeperMoving) {
			if (goalkeeper.position.x < goalkeeperTargetPosition) {
				goalkeeper.position.x += goalkeeperSpeed;
				if (goalkeeper.position.x > goalkeeperTargetPosition)
					goalkeeper.position.x = goalkeeperTargetPosition;
			} else if (goalkeeper.position.x > goalkeeperTargetPosition) {
				goalkeeper.position.x -= goalkeeperSpeed;
				if (goalkeeper.position.x < goalkeeperTargetPosition)
					goalkeeper.position.x = goalkeeperTargetPosition;
			}

			// Parar o movimento quando o goleiro alcançar o alvo
			if (goalkeeper.position.x == goalkeeperTargetPosition) {
				goalkeeperMoving = false;
			}
		}

		// Limpar a tela
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Usar o shader
		shader.use();

		// Renderizar o campo
		field.draw(shader);

		// Renderizar o jogador
		player.draw(shader);

		// Renderizar a bola
		ball.draw(shader);

		// Renderizar o goleiro
		goalkeeper.draw(shader);

		// Renderizar a seta (somente se a bola não estiver em movimento e for a vez do jogador)
		if (!ballIsMoving && playerTurn) {
			arrow.draw(shader);
		}

		// Trocar buffers e verificar eventos
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Exibir resultado final
	std::cout << "Fim do jogo!" << std::endl;
	std::cout << "Pontuação do Jogador: " << playerScore << std::endl;
	std::cout << "Pontuação do Goleiro: " << goalkeeperScore << std::endl;

	if (playerScore > goalkeeperScore) {
		std::cout << "Você venceu!" << std::endl;
	} else if (playerScore < goalkeeperScore) {
		std::cout << "Você perdeu!" << std::endl;
	} else {
		std::cout << "Empate!" << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, Sprite &arrow) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !ballIsMoving) {
        // Parar a seta
        movingRight = false;
        arrowSpeed = 0.0f; // Parar a movimentação da seta

        // Armazenar a direção do chute
        targetDirection = arrow.position.x;

        // Iniciar o movimento da bola
        ballIsMoving = true;

        // Definir a direção do goleiro
        int randomDirection = rand() % 3; // Gera um número entre 0, 1, 2
        if (randomDirection == 0) {
            goalkeeperTargetPosition = -0.3f; // Esquerda
        } else if (randomDirection == 1) {
            goalkeeperTargetPosition = 0.0f;  // Centro
        } else {
            goalkeeperTargetPosition = 0.3f;  // Direita
        }

        goalkeeperMoving = true; // Iniciar o movimento do goleiro
    }
}

