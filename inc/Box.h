#ifndef _BOX_H_
#define _BOX_H_

#include "Object.h"

class Box: public Object
{
    public:
        Box(glm::vec3 obj_pos, glm::vec3 obj_size = glm::vec3(1.0f, 1.0f, 1.0f),
        float rot_angle = 0.0f, glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f), //default rotate around z axis
        const float& a = 0.5f, const float& b = 0.5f, const float& c = 0.5f);
        
        virtual void render();
};

#endif