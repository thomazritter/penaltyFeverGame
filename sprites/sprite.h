#ifndef SPRITE_H
#define SPRITE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../shader/Shader.h"
#include "../Dependencies/stb_image/stb_image.h"

class Sprite {
public:
    unsigned int textureID;
    glm::vec2 position;
    glm::vec2 size;

    Sprite(unsigned int texture, glm::vec2 pos, glm::vec2 size) {
        this->textureID = texture;
        this->position = pos;
        this->size = size;

        // Aqui, configuramos os vértices do sprite, VAO, VBO, etc.
        // Isso é necessário para renderizar o sprite no OpenGL.
    }

    void draw(Shader &shader) {
        glBindTexture(GL_TEXTURE_2D, this->textureID);
        // Renderiza usando OpenGL, aplicando transformações e a textura.
    }
};

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Definir parâmetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carregar imagem
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB; // Suporta RGB e RGBA

        // Criar textura
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Erro ao carregar a textura: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

#endif
