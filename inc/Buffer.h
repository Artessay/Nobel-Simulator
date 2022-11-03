#pragma once
#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "GLObject.h"
#include <glad/glad.h>

class Buffer: public GLObject
{
    public:
        Buffer();
        ~Buffer();
        
        /**
         * @brief 
         * 
         * @param target the type of the buffer we want to copy data into:
         * @param size the size of the data (in bytes) we want to pass to the buffer
         * @param data the actual data we want to send
         * @param usage 
         * specifies how we want the graphics card to manage the given data. This can take 3 forms: \n
         * - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
         * - GL_STATIC_DRAW: the data is set only once and used many times.
         * - GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
         */
        void bind(GLenum target, GLsizeiptr size, const void *data, GLenum usage);

        void unbind(GLenum target);
};

#endif