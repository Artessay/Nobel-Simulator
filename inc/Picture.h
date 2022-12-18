#ifndef _PICTURE_H_
#define _PICTURE_H_

#include "Object.h"

class Picture: public Object
{
    public:
        Picture(const float& a = 1.0f, const float& b = 1.0f);
        
        virtual void render();
};

#endif