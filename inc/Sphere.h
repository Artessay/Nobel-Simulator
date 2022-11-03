#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Object.h"

class Sphere: public Object
{
    public:
        Sphere(const float& r = 1.0f, int sectors = 36);

		virtual void render();
    
    protected:
        virtual void bind();

        float radius;
};

#endif