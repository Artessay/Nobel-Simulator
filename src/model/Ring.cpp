#include <cmath>
#include "Ring.h"

#define PI 3.1415926

Ring::Ring(const float& R, const unsigned int& divisor)
{
    // higher divisor make circle more round

    for (unsigned int i = 0, half = (divisor>>1); i < divisor; ++i)
    {
        const float angle = i * PI / half;
        const float xi = R * cosf(angle);
        const float zi = R * sinf(angle);
        
        vertices.push_back(xi);
        vertices.push_back(0.0f);
        vertices.push_back(zi);
    }

    // bind
    bind();
}

void Ring::render()
{
    VAO.bind();
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size()/3);
}