#include <cmath>
#include <vector>
#include <iostream>
#include "BezierSurface.h"
#include "Bezier.h"
using namespace std;

#define PI 3.1415926

BezierSurface::BezierSurface(float controlPoints[], const int& step, const int& prec)
{
    // higher divisor make circle more round

    // for (unsigned int i = 0, half = (divisor>>1); i < divisor; ++i)
    // {
    //     const float angle = i * PI / half;
    //     const float xi = R * cosf(angle);
    //     const float zi = R * sinf(angle);
        
    //     vertices.push_back(xi);
    //     vertices.push_back(0.0f);
    //     vertices.push_back(zi);
    // }

    BezierFace myBezier = BezierFace(3, controlPoints, 100);
    // vector<float> pvalues;  //顶点坐标
    // vector<float> tvalues;  //纹理坐标
    // vector<float> nvalues;  //法线
    indices = myBezier.getIndices();
    vector<glm::vec3> verts = myBezier.getVertices();
    // vector<glm::vec2> tex = myBezier.getTexCoords();
    // vector<glm::vec3> norm = myBezier.getNormals();
    for (int i = 0; i < myBezier.getNumIndices(); i++)
    {
      vertices.push_back(verts[indices[i]].x);
      vertices.push_back(verts[indices[i]].y);
      vertices.push_back(verts[indices[i]].z);
      // tvalues.push_back(tex[ind[i]].s);
      // tvalues.push_back(tex[ind[i]].t);

      // nvalues.push_back(norm[ind[i]].x);
      // nvalues.push_back(norm[ind[i]].y);
      // nvalues.push_back(norm[ind[i]].z);
      // cout << i << " " << verts[indices[i]].x << " " << verts[indices[i]].y << " " << verts[indices[i]].z << endl;
    }

    // bind
    bind();
}

void BezierSurface::render()
{
    VAO.bind();
    // glDrawArrays(GL_LINE_LOOP, 0, vertices.size()/3);
	  glDrawArrays(GL_TRIANGLES, 0, indices.size());
}