#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Bomb.h"
#include "Picture.h"
#include <windows.h>

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

int Bomb::bomb_number = 0;
std::set<Bomb*> Bomb::bombSet;
std::vector <glm::vec3> Bomb::boomed_bombSet;

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

void Bomb::draw(Shader& shader, Texture &bomb_texture)
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
            glm::vec3 position = pBomb->position;
            delete pBomb;
            pBomb->explode(shader, position);
            Bomb::boomed_bombSet.push_back(position);
            
        }

        bomb_texture.use();
        glm::mat4 model_bomb;
        model_bomb = glm::translate(model_bomb, pBomb->position);
        
        shader.setMat4("model", model_bomb);

        static Sphere sphere(0.25f);
        sphere.render();
    }
Texture ruin_texture("res/textures/ruin.png");
ruin_texture.use();
    auto boomed_it = Bomb::boomed_bombSet.begin();
    while( boomed_it != Bomb::boomed_bombSet.end() ){
        Picture ruin;
        glm::mat4 model_ruin;
        model_ruin = glm::translate(model_ruin, glm::vec3((*boomed_it).x, (*boomed_it).y - 0.63f, (*boomed_it).z));
        model_ruin = glm::rotate(model_ruin, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader.setMat4("model", model_ruin);
        ruin.render();
        boomed_it++;
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

void Bomb::explode(Shader& shader,glm::vec3 position)
{
    cout << "BOMB!" << endl;
    
    Texture exp1_texture("res/textures/burst.png");
    exp1_texture.use();
    Picture smoke;

        float delta_time = 0.5f;
        glm::mat4 model_bomb;
        model_bomb = glm::translate(model_bomb, position);
        model_bomb = glm::scale(model_bomb, 
                 glm::vec3(1.0f + delta_time, 1.0f + delta_time, 1.0f + delta_time));
        shader.setMat4("model", model_bomb);
        smoke.render();
        
        model_bomb = glm::rotate(model_bomb,glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model_bomb);
        smoke.render();
        
        
}