#ifndef _CYLINDER_H_
#define _CYLINDER_H_

#include "Object.h"

class Cylinder: public Object
{
    public:
        Cylinder(glm::vec3 obj_pos, glm::vec3 obj_size = glm::vec3(1.0f, 1.0f, 1.0f),
        float rot_angle = 0.0f, glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f), //default rotate around z axis
        const float& R = 0.5, const float& H = 1.0, const unsigned int& sectors = 1024);

		virtual void render();
    
    protected:
        virtual void bind();

        // float radius;
};

#endif