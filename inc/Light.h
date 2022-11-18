#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Object.h"

/**
 * @brief Light Object
 * 
 */
class Light: public Object
{
    public:
        Light();

        virtual void render();
    
    protected:
        virtual void bind();
};

#endif