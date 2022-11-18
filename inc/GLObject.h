#pragma once
#ifndef _GL_OBJECT_H_
#define _GL_OBJECT_H_

class GLObject
{
    public:
        GLObject();
        GLObject(unsigned int id);
        GLObject(const GLObject& obj);
        unsigned int getID() const  { return ID; }
        
    protected:
        unsigned int ID;
};

#endif