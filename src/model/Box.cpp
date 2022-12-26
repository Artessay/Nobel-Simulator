#include "Box.h"

Box:: Box(glm::vec3 obj_pos, glm::vec3 obj_size, float rot_angle, glm::vec3 rot_axis, 
        const float& a, const float& b, const float& c)
:Object(obj_pos, obj_size, rot_angle, rot_axis)
{
    vertices = {
        // positions // normals          // texture coords
        -a, -b, -c,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         a, -b, -c,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         a,  b, -c,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         a,  b, -c,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -a,  b, -c,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -a, -b, -c,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -a, -b,  c,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         a, -b,  c,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         a,  b,  c,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         a,  b,  c,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -a,  b,  c,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -a, -b,  c,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -a,  b,  c, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -a,  b, -c, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -a, -b, -c, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -a, -b, -c, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -a, -b,  c, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -a,  b,  c, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         a,  b,  c,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         a,  b, -c,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         a, -b, -c,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         a, -b, -c,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         a, -b,  c,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         a,  b,  c,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -a, -b, -c,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         a, -b, -c,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         a, -b,  c,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         a, -b,  c,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -a, -b,  c,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -a, -b, -c,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -a,  b, -c,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         a,  b, -c,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         a,  b,  c,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         a,  b,  c,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -a,  b,  c,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -a,  b, -c,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    indices = {
        0,  0,
        1,  0,
        1,  1,
        1,  1,
        0,  1,
        0,  0,

        0,  0,
        1,  0,
        1,  1,
        1,  1,
        0,  1,
        0,  0,

        1,  0,
        1,  1,
        0,  1,
        0,  1,
        0,  0,
        1,  0,

        1,  0,
        1,  1,
        0,  1,
        0,  1,
        0,  0,
        1,  0,

        0,  1,
        1,  1,
        1,  0,
        1,  0,
        0,  0,
        0,  1,

        0,  1,
        1,  1,
        1,  0,
        1,  0,
        0,  0,
        0,  1

    };

    VAO.bind();

    VBO.bind(sizeof(float)*vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
    EBO.bind(sizeof(unsigned int)*indices.size(), indices.data(), GL_DYNAMIC_DRAW);

    // glBindVertexArray(Box::VAO.getID());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    VBO.unbind();
    VAO.unbind();
}

void Box::render(){
    VAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}