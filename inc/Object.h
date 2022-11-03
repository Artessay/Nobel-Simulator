#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"

#include <vector>

class Object
{
	public:
		Object();

		virtual ~Object();

		virtual void render();
		// virtual void resize();
		// float* getVertices() const;
		// unsigned int getSize() const;
		// unsigned int getVertexNumber() const;
		// void vertexInfoPrint() const;
		
	protected:
		/** Default bind function, draw static object, can be used only for vertices with 3 dimension for one point */
		virtual void bind();

		/** Default bind function. Can be used only for vertices with 3 dimension */
		virtual void bind(GLenum usage);

		std::vector<float> vertices;
		std::vector<int> indices;
		std::vector<float> textureCoordinate;

		VertexArray VAO;
		VertexBuffer VBO;
		ElementBuffer EBO;
};

#endif