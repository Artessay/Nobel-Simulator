#ifndef _BEZIERSURFACE_H_
#define _BEZIERSURFACE_H_

#include "Object.h"

class BezierSurface: public Object
{
    public:
        BezierSurface(float controlPoints[], const int& step = 3, const int& prec = 100);
        
        virtual void render();
};

#endif