#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Bomb.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

int Bomb::bomb_number = 0;
std::set<Bomb*> Bomb::bombSet;

Sphere Bomb::shape = Sphere(0.25f);
const float Bomb::bomb_velocity = 5.0;
const float Bomb::bomb_initial_height = 0.5;

glm::vec3 Bomb::bombPositions[MAX_BOMBS];

int Bomb::getBombNumber()
{
    return bomb_number;
}

glm::vec3 Bomb::getBombPosition(int index)
{
    if (index < 0 || index >= bomb_number)
    {
        std::cout << "program error, debug needed" << std::endl;
        return glm::vec3(0);
    }

    return Bomb::bombPositions[index];
}

bool Bomb::placeBomb(glm::vec3 bombPosition, glm::vec3 bombFront)
{
    if (bomb_number < MAX_BOMBS)
    {
        Bomb::bombPositions[bomb_number++] = bombPosition;
        bombSet.insert(new Bomb(bombPosition, bombFront));
        return true;
    }
    else
    {
        return false;
    }
}

void Bomb::draw(Shader& shader)
{
    for (auto pBomb: bombSet)
    {
        float delta_time = glfwGetTime() - pBomb->start_time;
        glm::vec3 show_place = pBomb->position + bomb_velocity * pBomb->front;
        show_place.y = show_place.y - delta_time * delta_time;
        if (show_place.y < 0.25)
        {
            show_place.y = 0.25;
        }

        glm::mat4 model_bomb;
        model_bomb = glm::translate(model_bomb, show_place);
        
        shader.setMat4("model", model_bomb);
        Bomb::shape.render();
    }
}

Bomb::Bomb(glm::vec3 bombPosition, glm::vec3 bombFront)
:   start_time(static_cast<float>(glfwGetTime())), 
    position(bombPosition),
    front(bombFront)
{
    static unsigned int ID_generator = 0;
    
    bomb_ID = ID_generator;
    ++ID_generator;
}