#ifndef _CYLINDER_H_
#define _CYLINDER_H_

#include "Object.h"

class Cylinder: public Object
{
    public:
        Cylinder(const float& R = 0.5, const float& H = 1.0, const unsigned int& divisor = 2048);
        
        virtual void render();
};

#endif