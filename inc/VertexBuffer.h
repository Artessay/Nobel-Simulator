#pragma once
#ifndef _VERTEX_BUFFER_H_
#define _VERTEX_BUFFER_H_

#include "Buffer.h"

class VertexBuffer: public Buffer
{
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
            Buffer::bind(GL_ARRAY_BUFFER, size, data, usage);
        }

        /**
         * @brief the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object 
         * so afterwards we can safely unbind
         * 
         */
        virtual void unbind()
        {
            Buffer::unbind(GL_ARRAY_BUFFER);
        }
};


#endif