#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Bomb.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

int Bomb::bomb_number = 0;
std::set<Bomb*> Bomb::bombSet;

const static float gravity = 0.098;
const float Bomb::bomb_velocity = 2.5;

// glm::vec3 Bomb::bombPositions[MAX_BOMBS];

int Bomb::getBombNumber()
{
    return bomb_number;
}

// glm::vec3 Bomb::getBombPosition(int index)
// {
//     if (index < 0 || index >= bomb_number)
//     {
//         std::cout << "program error, debug needed" << std::endl;
//         return glm::vec3(0);
//     }

//     return Bomb::bombPositions[index];
// }

bool Bomb::placeBomb(glm::vec3 bombPosition, glm::vec3 bombFront)
{
    if (bomb_number < MAX_BOMBS)
    {
        // Bomb::bombPositions[bomb_number++] = bombPosition;
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
    set<Bomb*>::const_iterator it = Bomb::bombSet.begin();
    while (it != Bomb::bombSet.end())
    {
        auto pBomb = *it; ++it;
        float delta_time = glfwGetTime() - pBomb->start_time;
        pBomb->position = pBomb->start_position + delta_time * bomb_velocity * pBomb->front;
        pBomb->position.y = pBomb->position.y - gravity * delta_time * delta_time;
        if (pBomb->position.y < 0.25)
        {
            pBomb->position.y = 0.25;
        }

        if (delta_time > 2.0f && pBomb->position.y == 0.25)
        {
            bombSet.erase(pBomb);
            --bomb_number;
            pBomb->explode();
            delete pBomb;
        }

        glm::mat4 model_bomb;
        model_bomb = glm::translate(model_bomb, pBomb->position);
        
        shader.setMat4("model", model_bomb);

        static Sphere sphere(0.25f);
        sphere.render();
    }
}

Bomb::Bomb(glm::vec3 bombPosition, glm::vec3 bombFront)
:   start_time(static_cast<float>(glfwGetTime())), 
    start_position(bombPosition),
    position(bombPosition),
    front(bombFront)
{
    static unsigned int ID_generator = 0;
    
    bomb_ID = ID_generator;
    ++ID_generator;
}

void Bomb::explode()
{
    cout << "BOMB!" << endl;
}