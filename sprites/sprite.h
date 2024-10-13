#ifndef SPRITE_H
#define SPRITE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../shader/Shader.h"
#include "../Dependencies/stb_image/stb_image.h"
#include <iostream>

struct Sprite
{
    GLuint VAO;
    GLuint texID;
    glm::vec3 position;
    glm::vec3 dimensions;
    float angle;
    int nAnimations, nFrames;
    int iAnimation, iFrame;
    glm::vec2 d;         // Size of each frame in texture coordinates
    glm::vec2 offsetTex; // Offset in texture coordinates for the current frame
    float FPS;
    float lastTime;

    // Function to update the frame offset based on the current animation and frame
    void updateFrame(int newFrame)
    {
        iFrame = newFrame;
        offsetTex.s = iFrame * d.s;
        offsetTex.t = iAnimation * d.t;
    }

    void setupSprite(int texID, glm::vec3 position, glm::vec3 dimensions, int nFrames, int nAnimations);
};

void Sprite::setupSprite(int texID, glm::vec3 position, glm::vec3 dimensions, int nFrames, int nAnimations)
{
    this->texID = texID;
    this->dimensions = dimensions;
    this->position = position;
    this->nAnimations = nAnimations;
    this->nFrames = nFrames;
    iAnimation = 0;

    d.s = 1.0 / (float)nFrames;
    d.t = 1.0 / (float)nAnimations;

    // Initialize the texture offset for the first frame
    updateFrame(0);
    // Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
    // sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
    // Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
    // Pode ser arazenado em um VBO único ou em VBOs separados
    GLfloat vertices[] = {
        // x   y     z    s     		t
        // T0
        -0.5, -0.5, 0.0, 0.0, 0.0, // V0
        -0.5, 0.5, 0.0, 0.0, d.t,  // V1
        0.5, -0.5, 0.0, d.s, 0.0,  // V2
        0.5, 0.5, 0.0, d.s, d.t    // V3
    };

    GLuint VBO, VAO;
    // Geração do identificador do VBO
    glGenBuffers(1, &VBO);
    // Faz a conexão (vincula) do buffer como um buffer de array
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Envia os dados do array de floats para o buffer da OpenGl
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Geração do identificador do VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);
    // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
    // e os ponteiros para os atributos
    glBindVertexArray(VAO);
    // Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando:
    //  Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
    //  Numero de valores que o atributo tem (por ex, 3 coordenadas xyz)
    //  Tipo do dado
    //  Se está normalizado (entre zero e um)
    //  Tamanho em bytes
    //  Deslocamento a partir do byte zero

    // Atributo 0 - Posição - x, y, z
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Atributo 1 - Coordenadas de textura - s, t
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
    // atualmente vinculado - para que depois possamos desvincular com segurança
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
    glBindVertexArray(0);
    this->VAO = VAO;
    FPS = 12.0;
    lastTime = 0.0;
}

int loadTexture(std::string filePath, int &imgWidth, int &imgHeight)
{
    GLuint texID;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) // jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else // png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << filePath << std::endl;
    }

    return texID;
}

void drawSprite(Sprite &spr, GLuint shaderID)
{
    glBindVertexArray(spr.VAO);
    glBindTexture(GL_TEXTURE_2D, spr.texID);

    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, spr.position);
    model = glm::rotate(model, glm::radians(spr.angle), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, spr.dimensions);

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), spr.offsetTex.s, spr.offsetTex.t);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

#endif