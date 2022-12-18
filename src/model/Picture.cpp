#include "Picture.h"

Picture:: Picture(const float& a , const float& b){
    vertices = {
        -a, -b, 0.0f,   0.0f,  0.0f,
         a, -b, 0.0f,   1.0f,  0.0f,
         a,  b, 0.0f,   1.0f,  1.0f,
         a,  b, 0.0f,   1.0f,  1.0f,
        -a,  b, 0.0f,   0.0f,  1.0f,
        -a, -b, 0.0f,   0.0f,  0.0f
    };

    VAO.bind();

    VBO.bind(sizeof(float)*vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    EBO.bind(sizeof(unsigned int)*indices.size(), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // EBO will also unbind after VBO unbind
    VBO.unbind();
    VAO.unbind();
}

void Picture::render(){
    VAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}