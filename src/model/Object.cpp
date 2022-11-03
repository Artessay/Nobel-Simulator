#include "Object.h"

Object::Object()
{
    ;
}

Object::~Object()
{
    ;
}

void Object::render()
{
    ;
}

void Object::bind()
{
    VAO.bind();
    VBO.bind(sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    VBO.unbind();
    VAO.unbind();
}

void Object::bind(GLenum usage)
{
    VAO.bind();
    VBO.bind(sizeof(float) * vertices.size(), vertices.data(), usage);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    VBO.unbind();
    VAO.unbind();
}