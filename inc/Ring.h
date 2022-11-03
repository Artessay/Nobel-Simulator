#ifndef _RING_H_
#define _RING_H_

#include "Object.h"

class Ring: public Object
{
    public:
        Ring(const float& R = 1.0, const unsigned int& divisor = 2048);
        
        virtual void render();
};

#endif