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

const static float gravity = 0.98;
const float Bomb::bomb_velocity = 2.5;

// glm::vec3 Bomb::bombPositions[MAX_BOMBS];

int Bomb::getBombNumber()
{
    return bomb_number;
}


bool Bomb::placeBomb(glm::vec3 bombPosition, glm::vec3 bombFront, glm::vec3 bombAhead)
{
    if (bomb_number < MAX_BOMBS)
    {
        bombSet.insert(new Bomb(bombPosition, bombFront, bombAhead));
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
        
        pBomb->position = pBomb->start_position + delta_time * bomb_velocity * pBomb->start_angle_cos * pBomb->ahead;
        pBomb->position.y = pBomb->position.y + delta_time * bomb_velocity * pBomb->start_angle_sin - 0.5 * gravity * delta_time * delta_time;
        
        if (pBomb->position.y < 0.15f)
        {
            pBomb->position.y = 0.15f;
        }

        // if (delta_time > 2.0f || pBomb->position.y == 0.15)
        if (pBomb->position.y == 0.15f)
        {
            bombSet.erase(pBomb);
            
            glm::vec3 position = pBomb->position;
            pBomb->explode(shader, position);
            delete pBomb;
            Bomb::boomed_bombSet.push_back(position);
            continue;
        }

        glm::mat4 model_bomb;
        model_bomb = glm::translate(model_bomb, pBomb->position);
        
        shader.setMat4("model", model_bomb);

        static Sphere sphere(0.15f);
        sphere.render();
    }
    
}

void Bomb::drawRuin(Shader& shader)
{
    
    auto boomed_it = Bomb::boomed_bombSet.begin();
    while( boomed_it != Bomb::boomed_bombSet.end() ){
        Picture ruin;
        glm::mat4 model_ruin;
        model_ruin = glm::translate(model_ruin, glm::vec3((*boomed_it).x, (*boomed_it).y - 0.53f, (*boomed_it).z));
        model_ruin = glm::rotate(model_ruin, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader.setMat4("model", model_ruin);
        ruin.render();
        boomed_it++;
    }
}

Bomb::Bomb(glm::vec3 bombPosition, glm::vec3 bombFront, glm::vec3 bombAhead)
:   start_time(static_cast<float>(glfwGetTime())), 
    start_position(bombPosition),
    position(bombPosition),
    front(bombFront),
    ahead(bombAhead)
{
    static unsigned int ID_generator = 0;
    
    bomb_ID = ID_generator;
    ++ID_generator;

    ++bomb_number;
    start_angle_cos = glm::dot(front, ahead) / sqrt(glm::dot(front, front) * glm::dot(ahead, ahead));
    start_angle_sin = (1 - start_angle_cos * start_angle_cos) * 
                    (front.y >= 0 ? 1 : -1);
}

Bomb::~Bomb()
{
    --bomb_number;
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