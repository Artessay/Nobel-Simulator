#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Object.h"

class Light: public Object
{
    public:
        Light();

        virtual void render();
    
    protected:
        virtual void bind();
};

#endif