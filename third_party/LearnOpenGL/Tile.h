/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Shader.h"


class Tile
{
public:
    // Constructor (inits shaders/shapes)
    Tile(const Shader &shader);
    // Destructor
    virtual ~Tile();
    // Renders a defined quad textured with given sprite
    void Draw(const Texture &texture, glm::vec2 position,
    		glm::vec2 size = glm::vec2(10.0f, 10.0f),
			float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
private:
    // Render state
    Shader       shader;
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};
