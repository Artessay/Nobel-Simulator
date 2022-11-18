#ifndef _ILLUMINANT_H_
#define _ILLUMINANT_H_

#include "Object.h"

/**
 * @brief Light Object
 * 
 */
class Illuminant: public Object
{
    public:
        Illuminant();

        virtual void render();
    
    protected:
        virtual void bind();
};

#endif