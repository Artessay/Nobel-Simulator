#include <cmath>
#include "Cylinder0.h"

#define PI 3.1415926

Cylinder::Cylinder(glm::vec3 obj_pos, glm::vec3 obj_size, float rot_angle, glm::vec3 rot_axis, 
    const float& R, const float& H, const unsigned int& divisor)
:Object(obj_pos, obj_size, rot_angle, rot_axis)
{
    // higher divisor make circle more round

    for (unsigned int i = 0, half = (divisor>>1); i <= divisor; ++i)
    {
        const float angle = i * PI / half;
        const float xi = R * cosf(angle);
        const float zi = R * sinf(angle);

        float s = (float)i / divisor;
        float t = (float)i / divisor;
        
        vertices.push_back(xi);
        vertices.push_back(0.0f - H/2.0f);
        vertices.push_back(zi);
        
        vertices.push_back(s);
        vertices.push_back(t);

        vertices.push_back(xi);
        vertices.push_back(0.0f + H/2.0f);
        vertices.push_back(zi);
        
        vertices.push_back(s);
        vertices.push_back(t);
    }
    
    for (unsigned int i = 0, half = (divisor>>1); i <= divisor; ++i)
    {
        const float angle = i * PI / half;
        const float xi = R * cosf(angle);
        const float zi = R * sinf(angle);
        
        vertices.push_back(xi);
        vertices.push_back(0.0f - H/2.0f);
        vertices.push_back(zi);
        
        float s = (float)i / divisor;
        float t = (float)i / divisor;

        vertices.push_back(s);
        vertices.push_back(t);
    }

    for (unsigned int i = 0, half = (divisor>>1); i <= divisor; ++i)
    {
        const float angle = i * PI / half;
        const float xi = R * cosf(angle);
        const float zi = R * sinf(angle);
        
        vertices.push_back(xi);
        vertices.push_back(0.0f + H/2.0f);
        vertices.push_back(zi);
        
        float s = (float)i / divisor;
        float t = (float)i / divisor;
        
        vertices.push_back(s);
        vertices.push_back(t);
    }

    // bind
    bind();
}

void Cylinder::bind()
{
    VAO.bind();
    VBO.bind(sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    VBO.unbind();
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