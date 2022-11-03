#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#include "Object.h"

class Circle: public Object
{
    public:
        Circle(const float& R = 1.0, const unsigned int& divisor = 96);
        
        virtual void render();
};

#endif