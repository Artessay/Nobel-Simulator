#ifndef _CUBE_H_
#define _CUBE_H_

#include "Object.h"

class Cube: public Object
{
    public:
        Cube();
        
        virtual void render();
};

#endif