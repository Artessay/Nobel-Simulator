#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "GLObject.h"

class Texture: public GLObject
{
    public:
        Texture(const char * texturePath);

        void use();
};

#endif