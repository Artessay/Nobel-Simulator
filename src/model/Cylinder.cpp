#include <cmath>
#include "Cylinder.h"

#define PI 3.1415926

Cylinder::Cylinder(glm::vec3 obj_pos, glm::vec3 obj_size, float rot_angle, glm::vec3 rot_axis, 
    const float& R, const float& H, const unsigned int& divisor)
:Object(obj_pos, obj_size, rot_angle, rot_axis)
{
    // higher divisor make circle more round

    for (unsigned int i = 0, half = (divisor>>1); i < divisor; ++i)
    {
        const float angle = i * PI / half;
        const float xi = R * cosf(angle);
        const float zi = R * sinf(angle);
        
        vertices.push_back(xi);
        vertices.push_back(0.0f - H/2.0f);
        vertices.push_back(zi);

        normals.push_back(xi);
        normals.push_back(-1);
        normals.push_back(zi);

        textureCoordinate.push_back(i);
        textureCoordinate.push_back(0);
        
        vertices.push_back(xi);
        vertices.push_back(0.0f + H/2.0f);
        vertices.push_back(zi);

        normals.push_back(xi);
        normals.push_back(1);
        normals.push_back(zi);

        textureCoordinate.push_back(i);
        textureCoordinate.push_back(1);
    }
    
    for (unsigned int i = 0, half = (divisor>>1); i < divisor; ++i)
    {
        const float angle = i * PI / half;
        const float xi = R * cosf(angle);
        const float zi = R * sinf(angle);
        
        vertices.push_back(xi);
        vertices.push_back(0.0f - H/2.0f);
        vertices.push_back(zi);

        normals.push_back(xi);
        normals.push_back(-1);
        normals.push_back(zi);

        textureCoordinate.push_back(i);
        textureCoordinate.push_back(0);
    }

    for (unsigned int i = 0, half = (divisor>>1); i < divisor; ++i)
    {
        const float angle = i * PI / half;
        const float xi = R * cosf(angle);
        const float zi = R * sinf(angle);
        
        vertices.push_back(xi);
        vertices.push_back(0.0f + H/2.0f);
        vertices.push_back(zi);

        normals.push_back(xi);
        normals.push_back(1);
        normals.push_back(zi);

        textureCoordinate.push_back(i);
        textureCoordinate.push_back(1);
    }

    // bind
    bind();
}

void Cylinder::bind()
{
    VAO.bind();

    VBO.bind(sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    EBO.bind(sizeof(unsigned int)*indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    VBO.unbind();

    normalVBO.bind(sizeof(float)*normals.size(), normals.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    normalVBO.unbind();

    textureVBO.bind(sizeof(float)*textureCoordinate.size(), textureCoordinate.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    textureVBO.unbind();

    VAO.unbind();
}

void Cylinder::render()
{
    VAO.bind();
    GLsizei num = vertices.size() / 3 / 4;  // number of points on one circle
    glDrawArrays(GL_TRIANGLE_STRIP, 0, num * 2);
    glDrawArrays(GL_TRIANGLE_FAN, num * 2 + 1, num);
    glDrawArrays(GL_TRIANGLE_FAN, num * 3 + 1, num);
}