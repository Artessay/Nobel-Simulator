#ifndef _BOX_H_
#define _BOX_H_

#include "Object.h"

class Box: public Object
{
    public:
        Box(const float& a = 0.5f, const float& b = 0.5f, const float& c = 0.5f);
        
        virtual void render();
};

#endif