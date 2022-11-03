#include <cmath>
#include "Sphere.h"

static const float PI = 3.1415926;

Sphere::Sphere(const float& r, int sectors)
: radius(r)
{
    // divisor
    int stacks = (sectors >> 1);
    float step = 2 * PI / sectors;

    // vertex and texture array
    for (int i = 0; i <= stacks; ++i)
    {
        float phi = (float)(i * step);       // starting from 0 to pi

        float xy = radius * sinf(phi);       // r * cos(u)
        float z  = radius * cosf(phi);       // r * sin(u)

        // add (sectors+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectors; ++j)
        {
            float theta = j * step;          // starting from 0 to 2pi

            float x = xy * cosf(theta);      // r * cos(u) * cos(v)
            float y = xy * sinf(theta);		 // r * cos(u) * sin(v)

            // vertex position (x, y, z)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // vertex texture coordinate (s, t) range between [0, 1]
            float s = (float)j / sectors;
            float t = (float)i / stacks;

            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    // index array
    for (int i = 0; i < stacks; ++i)
    {
        int k1 = i * (sectors + 1);     // beginning of current stack
        int k2 = k1 + sectors + 1;      // beginning of next stack

        for (int j = 0; j < sectors; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stacks - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    bind();
}

void Sphere::bind()
{
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    VAO.bind();

    VBO.bind(sizeof(float)*vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    EBO.bind(sizeof(unsigned int)*indices.size(), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // EBO will also unbind after VBO unbind
    VBO.unbind();
    VAO.unbind();
}

void Sphere::render()
{
    VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}