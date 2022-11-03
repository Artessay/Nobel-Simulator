#pragma once
#ifndef _ELEMENT_BUFFER_H_
#define _ELEMENT_BUFFER_H_

#include "Buffer.h"

class ElementBuffer: public Buffer
{
public:
    public:
        /**
         * @brief 
         * 
         * @param size the size of the data (in bytes) we want to pass to the buffer
         * @param data the actual data we want to send
         * @param usage 
         * specifies how we want the graphics card to manage the given data. This can take 3 forms: \n
         * - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
         * - GL_STATIC_DRAW: the data is set only once and used many times.
         * - GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
         */
        virtual void bind(GLsizeiptr size, const void *data, GLenum usage = GL_STATIC_DRAW)
        {
            Buffer::bind(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
        }

        /**
         * @brief do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
         * 
         */
        virtual void unbind()
        {
            Buffer::unbind(GL_ARRAY_BUFFER);
        }
};

#endif