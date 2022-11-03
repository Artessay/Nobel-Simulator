#include <GLObject.h>

GLObject::GLObject()
: ID(0) 
{  }

GLObject::GLObject(unsigned int id) 
: ID(id) 
{  }

GLObject::GLObject(const GLObject& obj)
: ID(obj.ID)
{  }