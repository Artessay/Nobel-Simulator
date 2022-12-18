#ifndef _BOX_H_
#define _BOX_H_

#include "Object.h"

class Box: public Object
{
    public:
        Box(const float& a = 1.0f, const float& b = 1.0f, const float& c = 1.0f);
        
        virtual void render();
};

#endif