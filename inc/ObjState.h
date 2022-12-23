#ifndef _OBJSTATE_H_
#define _OBJSTATE_H_

#include <glm/glm.hpp>

enum ObjType{box, sphere, cylinder};
class ObjState
{
private:
  int index;
  ObjType type;
  glm::vec3 position;	//position of center
  glm::vec3 size;			//scale
  float angle;				//angle of rotation
  glm::vec3 axis;			//axis of rotation
public:
  ObjState(){}
  ~ObjState(){}
  void setState(int _ind, ObjType _type, glm::vec3 _pos, 
                glm::vec3 _size, float _an, glm::vec3 _ax)
  {
    index = _ind;
    type = _type;
    position = _pos;
    size = _size;
    angle = _an;
    axis = _ax;
  }
  ObjType getType(){return type;}
  glm::vec3 getPos(){return position;}
  glm::vec3 getSize(){return size;}
  float getAngle(){return angle;}
  glm::vec3 getAxis(){return axis;}
};


#endif