#pragma once
#ifndef _VERTEX_ARRAY_H_
#define _VERTEX_ARRAY_H_

#include "GLObject.h"
#include <GLad/glad.h>

class VertexArray: public GLObject
{
    public:
        VertexArray(/* args */)
        {
            glGenVertexArrays(1, &ID);
        }
        virtual ~VertexArray()
        {
            glDeleteVertexArrays(1, &ID);
        }

        void bind()
        {
            glBindVertexArray(ID);
        }

        /**
         * @brief You can unbind the VAO after use so other VAO calls won't accidentally modify this VAO. 
         * But this rarely happens. 
         * Modifying other VAOs requires a call to glBindVertexArray anyways 
         * so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
         * 
         */
        void unbind()
        {
            glBindVertexArray(0);
        }
};


#endif