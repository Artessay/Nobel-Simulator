#include <cmath>
#include <Circle.h>

#define PI 3.1415926

Circle::Circle(const float& R, const unsigned int& divisor)
{
    // higher divisor make circle more round
        
    unsigned int vertexNumber = divisor * 3 + 6;
    vertices.reserve(vertexNumber);

    // const unsigned int i_q = (divisor >> 3);
    const unsigned int quarter = (divisor >> 2);
    const unsigned int half = (divisor >> 1);
    const unsigned int third = quarter + half;

    vertices[0] = 0;  vertices[1] = 0;
    
    vertices[3] = vertices[3*divisor+3] = 0 + R;
    vertices[4] = vertices[3*half+4] = vertices[3*divisor+4] = 0;
    vertices[3*quarter+3] = vertices[3*third+3] = 0;
    vertices[3*quarter+4] = 0 + R;
    vertices[3*half+3] = 0 - R;
    vertices[3*third+4] = 0 - R;

    for (unsigned int i = 1; i < quarter; ++i)
    {
        const float angle = i * PI / half;
        const float xi = R * cosf(angle);
        const float yi = R * sinf(angle);
        
        vertices[3 * i + 3]     = 0 + xi;
        vertices[3 * i + 1 + 3] = 0 + yi;

        vertices[3 * (half    + i) + 3]     = 0 -xi;
        vertices[3 * (half    + i) + 1 + 3] = 0 -yi;

        vertices[3 * (quarter + i) + 3]     = 0 -yi;
        vertices[3 * (quarter + i) + 1 + 3] = 0 +xi;

        vertices[3 * (third   + i) + 3]     = 0 +yi;
        vertices[3 * (third   + i) + 1 + 3] = 0 -xi;
    }

    // z axis
    for (unsigned int i = 2; i < vertexNumber; i += 3)
        vertices[i] = 0;

    // bind
    bind();
}

void Circle::render()
{
    VAO.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size()/3);
    
}