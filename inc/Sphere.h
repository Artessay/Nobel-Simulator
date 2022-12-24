#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Object.h"

class Sphere: public Object
{
    public:
        Sphere(
            const float& r = 0.5f,
            glm::vec3 obj_pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 obj_size = glm::vec3(1.0f, 1.0f, 1.0f),
            float rot_angle = 0.0f, glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f),
            int sectors = 36
            );

		virtual void render();
    
    protected:
        virtual void bind();

        float radius;
};

#endif