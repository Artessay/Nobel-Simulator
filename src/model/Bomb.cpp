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



glm::vec3 Bomb::draw(Shader& shader)
{
    glm::vec3 ret = glm::vec3(0.0f,0.0f,0.0f);
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

        // if(pBomb->ifCollide()){cout << "collide!" << endl;}

        // if (delta_time > 2.0f || pBomb->position.y == 0.15)
        if (pBomb->position.y == 0.15f || pBomb->ifCollide())
        {
            bombSet.erase(pBomb);
            
            glm::vec3 position = pBomb->position;
            ret = pBomb->position;
            pBomb->explode(shader, position);
            delete pBomb;
            if(pBomb->position.y < 0.18f)
                Bomb::boomed_bombSet.push_back(position);
            continue;
        }

        glm::mat4 model_bomb;
        model_bomb = glm::translate(model_bomb, pBomb->position);
        
        shader.setMat4("model", model_bomb);

        static Sphere sphere(0.15f);
        sphere.render();
    }
    return ret;
}

void Bomb::drawRuin(Shader& shader)
{
    int sz = 0;//Bomb::boomed_bombSet.size();
    auto boomed_it = Bomb::boomed_bombSet.begin();
    while( boomed_it != Bomb::boomed_bombSet.end() ){
        Picture ruin;
        glm::mat4 model_ruin;
        
        model_ruin = glm::translate(model_ruin, glm::vec3((*boomed_it).x, -0.4f + 0.001f*sz, (*boomed_it).z));
        model_ruin = glm::rotate(model_ruin, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader.setMat4("model", model_ruin);
        ruin.render();
        sz++;
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
    // int num = objects.size();
    // cout << num << endl;
    // for(auto i = 0; i < objects.size(); i++){
    //     cout << i << " type = " << objects[i]->getType();
    //     cout << " posX = " << objects[i]->getPos().x;
    //     cout << " sizeX = " << objects[i]->getSize().x << endl;
    // }
    // cout << position.x << " " << position.y << " " << position.z << endl;
    // cout << "radius = " << Radius << endl;
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

void Bomb::setObjState(vector<ObjState*> _objs)
{
    objects = _objs;
    // for(auto i = 0; i < _objs.size(); i++){
    //     objects.push_back(_objs[i]);
    // }
}

bool Bomb::ifCollide()
{
    bool iscollide = false;
    for(auto i = 0; i < objects.size(); i++){
        switch (objects[i]->getType())
        {
        case ObjType::sphere :{
            iscollide |= ifCollideSphere(objects[i]);
            break;
        }
        case ObjType::cylinder :{
            iscollide |= ifCollideCylinder(objects[i]);
            break;
        }
        case ObjType::box :{
            iscollide |= ifCollideBox(objects[i]);
            break;
        }
        default:
            break;
        }
    }
    return iscollide;
}


bool Bomb::ifCollideSphere(ObjState *sphere)
{
    bool iscollide = false;
    float r = (sphere->getSize().x)/2.0f;
    glm::vec3 diff = position - sphere->getPos();
    // cout << diff.x << " " << diff.y << endl;
    float distance = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    // cout << "distance = " << distance << endl;
    if(distance <= (r + Radius)*(r + Radius)){
        iscollide = true;
        cout << "collide with sphere" << endl;
    }
    return iscollide;
}
bool Bomb::ifCollideCylinder(ObjState *cylinder)
{
    bool iscollide = false;
    float r = (cylinder->getSize().x)/2.0f;
    if(cylinder->getAngle() == 0.0f){
        float h = cylinder->getPos().y + (cylinder->getSize().y)/2.0f;
        glm::vec3 diff = position - cylinder->getPos();
        float dist_xz = diff.x * diff.x + diff.z * diff.z;
        // cout << "dist_xz = " << dist_xz << endl;
        if((position.y <= h && dist_xz <= (r + Radius)*(r + Radius))
        || (position.y <= h + Radius && dist_xz <= r * r)
        // (y-h)^2 + (sqrt(dist)-r)^2 <= R^2
        || ((position.y - h) * (position.y - h) + (sqrt(dist_xz) - r) * (sqrt(dist_xz) - r) <= Radius * Radius)){
            iscollide = true;
            cout << "collide with cylinder" << endl;
        }
    }else{
        float front = cylinder->getPos().z + (cylinder->getSize().y)/2.0f;
        float back = cylinder->getPos().z - (cylinder->getSize().y)/2.0f;
        //cout << "front = " << front << " back = " << back << endl;
        glm::vec3 diff = position - cylinder->getPos();
        float dist_xy = diff.x * diff.x + diff.y * diff.y;
        //cout << "dist_xy = " << dist_xy << endl;
        if((position.z <= front && position.z >= back && dist_xy <= (r + Radius)*(r + Radius))
        || (position.z <= front + Radius && position.z >= back - Radius && dist_xy <= r * r)
        || ((position.z - front) * (position.z - front) + (sqrt(dist_xy) - r) * (sqrt(dist_xy) - r) <= Radius * Radius)
        || ((position.z - back) * (position.z - back) + (sqrt(dist_xy) - r) * (sqrt(dist_xy) - r) <= Radius * Radius)){
            if(position.z <= front && position.z >= back && dist_xy <= (r + Radius)*(r + Radius)){
                cout << "1 ";
                cout << position.z << " ";}
            if((position.z <= front + Radius && position.z >= back - Radius && dist_xy <= r * r)){
                cout << "2 ";
                cout << position.z << " ";
            }
            if(((position.z - front) * (position.z - front) + (sqrt(dist_xy) - r) * (sqrt(dist_xy) - r) <= Radius * Radius))
                cout << "3 ";
            if(((position.z - back) * (position.z - back) + (sqrt(dist_xy) - r) * (sqrt(dist_xy) - r) <= Radius * Radius))
                cout << "4 ";
            iscollide = true;
            cout << "collide with cylinder" << endl;
        }
    }
    return iscollide;
}
bool Bomb::ifCollideBox(ObjState *box)
{
    bool iscollide = false;
    return iscollide;
}