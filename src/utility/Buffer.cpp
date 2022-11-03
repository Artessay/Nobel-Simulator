#include "Buffer.h"

Buffer::Buffer()
{
    // Generate Buffer
    glGenBuffers(1, &ID);
}

Buffer::~Buffer()
{
    // de-allocate resource
    glDeleteBuffers(1, &ID);
}

void Buffer::bind(GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
    // Bind GLObject into Buffer
    glBindBuffer(target, ID);  

    // copy user-defined data into the currently bound buffer
    glBufferData(target, size, data, usage);
}

void Buffer::unbind(GLenum target)
{
    glBindBuffer(target, 0);
}