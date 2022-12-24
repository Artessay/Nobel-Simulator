#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"

#include <vector>
#include <glm/glm.hpp>

class Object
{
	public:
		Object(
			glm::vec3 obj_pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 obj_size = glm::vec3(1.0f, 1.0f, 1.0f),
      float rot_angle = 0.0f, glm::vec3 rot_axis = glm::vec3(0.0f, 1.0f, 0.0f) //default rotate around z axis
		);

		virtual ~Object();

		virtual void render();
		glm::vec3 getPosition() { return position; }
		glm::vec3 getSize() { return size; }
		float getAngle() { return angle; }
		glm::vec3 getAxis() { return axis; }
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

		glm::vec3 position;	//position of center
		glm::vec3 size;			//scale
		float angle;				//angle of rotation
		glm::vec3 axis;			//axis of rotation
};

#endif