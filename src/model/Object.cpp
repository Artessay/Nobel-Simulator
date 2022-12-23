#include "Object.h"

Object::Object(glm::vec3 obj_pos, glm::vec3 obj_size, float rot_angle, glm::vec3 rot_axis)
{
    position = obj_pos;
    size = obj_size;
    angle = rot_angle;
    axis = rot_axis;
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