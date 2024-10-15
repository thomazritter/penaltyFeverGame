#include <iostream>
#include <string>
#include <assert.h>
#include <chrono>
#include <thread>
#include <irrKlang.h>

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
#include <scoreboard.h>

using namespace std;
using namespace irrklang;
using namespace glm;

// Setup IRRKLANG audio
ISoundEngine *SoundEngine = createIrrKlangDevice();

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

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
Scoreboard scoreboard;

// Controle para a seta de direção do chute
bool isVerticalArrowMoving = false;
bool isPlayerShooting = true;
bool isPlayerSelectingTarget = true;
bool isKickAnimationComplete = false;
bool isBallAnimationComplete = false;
bool wasSpacePressed = false;
bool isGoalkeeperAnimationComplete = false;

Coordinates selectedKickTarget = {0.0f, 0.0f};
GoalSection playerKickSection = LEFT_BOTTOM;
Coordinates goalkeeperTarget = {0.0f, 0.0f};
GoalSection goalkeeperDefenseSection = LEFT_BOTTOM;

bool showTarget = false;
bool playerHasSelectedDefenseTarget = false;
float circleDisplayTime = 5.0f;
float circleTimer = 0.0f;

// Resultado do chute
bool playerScored = false;
bool playerDefended = false;

// Placar da partida
int playerScore = 0;
int opponentScore = 0;

Coordinates getMouseClickCoordinates(GLFWwindow *window)
{
    Coordinates clickCoordinates = {0.0, 0.0};
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Get the window height to invert the y-coordinate
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        ypos = windowHeight - ypos;

        clickCoordinates.x = xpos;
        clickCoordinates.y = ypos;

        std::cout << "Mouse clicked at: (" << xpos << ", " << ypos << ")" << std::endl;
    }
    return clickCoordinates;
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
    goalkeeperDefenseSection = GoalSection::MIDDLE_BOTTOM;
    goalkeeper.defineTargetBySection(goalkeeperDefenseSection);
    isPlayerShooting = false;
    isPlayerSelectingTarget = false;
    isGoalkeeperAnimationComplete = false;
    playerHasSelectedDefenseTarget = false;
    isVerticalArrowMoving = false;
    isKickAnimationComplete = false;
    isBallAnimationComplete = false;
    showTarget = false;
    circleTimer = 0.0f;
}

bool isItGoal(GoalSection playerKickSection, GoalSection goalkeeperDefenseSection)
{
    switch (playerKickSection)
    {
    case LEFT_BOTTOM:
        if (goalkeeperDefenseSection == LEFT_BOTTOM)
        {
            return false;
        }
        else
        {
            return true;
        }
    case LEFT_TOP:
        if (goalkeeperDefenseSection == LEFT_TOP)
        {
            return false;
        }
        else
        {
            return true;
        }
    case MIDDLE_BOTTOM:
        if (goalkeeperDefenseSection == MIDDLE_BOTTOM)
        {
            return false;
        }
        else
        {
            return true;
        }
    case MIDDLE_TOP:
        if (goalkeeperDefenseSection == MIDDLE_TOP)
        {
            return false;
        }
        else
        {
            return true;
        }
    case RIGHT_BOTTOM:
        if (goalkeeperDefenseSection == RIGHT_BOTTOM)
        {
            return false;
        }
        else
        {
            return true;
        }
    case RIGHT_TOP:
        if (goalkeeperDefenseSection == RIGHT_TOP)
        {
            return false;
        }
        else
        {
            return true;
        }
    default:
        return false;
    }
}

void checkGameOver(GLFWwindow *window)
{
    if (playerScore >= 5 || opponentScore >= 5)
    {
        if (playerScore >= 5)
        {
            std::cout << "Player wins!" << std::endl;
        }
        else
        {
            std::cout << "Opponent wins!" << std::endl;
        }
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main()
{
    //using namespace std::this_thread; // sleep_for, sleep_until
    //using namespace std::chrono; // nanoseconds, system_clock, seconds

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
    const GLubyte *version = glGetString(GL_VERSION);   /* version as a string */
    std::cout << "Renderer: " << renderer << endl;
    std::cout << "OpenGL version supported " << version << endl;
    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    background.setupSprite();
    player.setupSprite();
    ball.setupSprite();
    goalkeeper.setupSprite();
    horizontalArrow.setupSprite();
    verticalArrow.setupSprite();
    target.setupSprite();
    scoreboard.setupSprite();

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

    //comecar audio
    //SoundEngine->play2D("soundtrack/crowd.mp3", true);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSprite(background.sprite, shaderID);
        drawSprite(goalkeeper.sprite, shaderID);
        drawSprite(ball.sprite, shaderID);
        drawSprite(player.sprite, shaderID);
        drawSprite(scoreboard.playerScoreboardSprite, shaderID);
        drawSprite(scoreboard.opponentScoreboardSprite, shaderID);

        if (isPlayerShooting)
        {
            if (isPlayerSelectingTarget)
            {
                if (!isVerticalArrowMoving)
                {
                    horizontalArrow.move();
                    drawSprite(horizontalArrow.sprite, shaderID);
                }
                else
                {
                    verticalArrow.move();
                    drawSprite(verticalArrow.sprite, shaderID);
                    drawSprite(horizontalArrow.sprite, shaderID);
                }

                if (keys[GLFW_KEY_SPACE] && !wasSpacePressed)
                {
                    if (!isVerticalArrowMoving)
                    {
                        selectedKickTarget.x = horizontalArrow.sprite.position.x;
                        isVerticalArrowMoving = true;
                    }
                    else
                    {
                        selectedKickTarget.y = verticalArrow.sprite.position.y;
                        isPlayerSelectingTarget = false;
                        playerKickSection = determineGoalSection(selectedKickTarget);
                        goalkeeperTarget = selectRandomTargetInsideGoal();
                        goalkeeperDefenseSection = determineGoalSection(goalkeeperTarget);
                        goalkeeper.defineTargetBySection(goalkeeperDefenseSection);
                        ball.setTarget(playerKickSection);
                        isKickAnimationComplete = false;
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
                    player.movePlayer(isKickAnimationComplete);
                }
                else
                {
                    if (!isBallAnimationComplete || !isGoalkeeperAnimationComplete)
                    {
                        ball.moveBall(isBallAnimationComplete);
                        goalkeeper.moveGoalkeeper(isGoalkeeperAnimationComplete);
                    }
                    else
                    {

                        playerScored = isItGoal(playerKickSection, goalkeeperDefenseSection);
                        if (playerScored)
                        {
                            playerScore++;
                            playerScored = false;
                            scoreboard.setupPlayerScore(playerScore);
                        }
                        resetPositions();
                        isPlayerShooting = false;
                        //sleep_for(seconds(2)); // Sleep before switching to the opponent's turn
                    }
                }
            }
        }
        else
        {
            if (!playerHasSelectedDefenseTarget && !isKickAnimationComplete)
            {
                goalkeeperTarget = getMouseClickCoordinates(window);
                if (determineGoalSection(goalkeeperTarget) != OUTSIDE)
                {
                    playerHasSelectedDefenseTarget = true;
                    goalkeeperDefenseSection = determineGoalSection(goalkeeperTarget);
                    goalkeeper.defineTargetBySection(goalkeeperDefenseSection);
                }
            }
            if (!showTarget)
            {
                selectedKickTarget = selectRandomTargetAnywhere();
                playerKickSection = determineGoalSection(selectedKickTarget);
                ball.setTarget(playerKickSection);
                target.setTargetBySection(playerKickSection);
                showTarget = true;
                isKickAnimationComplete = false;
            }
            else
            {
                circleTimer += 1.0f;
                if (circleTimer <= circleDisplayTime)
                {
                    drawSprite(target.sprite, shaderID);
                }
                if (!isKickAnimationComplete)
                {
                    player.movePlayer(isKickAnimationComplete);
                }
                else
                {
                    if (!isBallAnimationComplete || !isGoalkeeperAnimationComplete)
                    {
                        ball.moveBall(isBallAnimationComplete);
                        goalkeeper.moveGoalkeeper(isGoalkeeperAnimationComplete);
                    }
                    else
                    {
                        playerScored = isItGoal(playerKickSection, goalkeeperDefenseSection);
                        if (playerScored)
                        {
                            opponentScore++;
                            playerScored = false;
                            scoreboard.setupOpponentScore(opponentScore);
                        }
                        resetPositions();
                        isPlayerShooting = true;
                        isPlayerSelectingTarget = true;
                    }
                }
            }
        }

        checkGameOver(window);
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}