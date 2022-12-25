#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using namespace std;

// Constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) 
: Front(glm::vec3(0.0f, 0.0f, 1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM), FreeCamera(true)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
: Front(glm::vec3(0.0f, 0.0f, 1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM), FreeCamera(true)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    Position_old = Position;

    if (FreeCamera)
    {
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == SCROLL_FORWARD)
            if (Position.y > 70.0f)
                Position += Front * (1.0f * deltaTime);
        if (direction == SCROLL_BACKWARD)
            Position -= Front * (1.5f * deltaTime);
    }
    else
    {
        if (direction == FORWARD)
            Position += glm::vec3(0.0f, 0.0f, -1.0f) * velocity;
        if (direction == BACKWARD)
            Position -= glm::vec3(0.0f, 0.0f, -1.0f) * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == SCROLL_FORWARD)
            if (Position.y > 70.0f)
                Position += Front * (10000.0f * deltaTime);
        if (direction == SCROLL_BACKWARD)
            Position -= Front * (15000.0f * deltaTime);
    }

    physicalPositionCheck();
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    if (FreeCamera)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        } 
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);

    // always direct at the look at front
    Ahead.x = front.x;
    Ahead.z = front.z;
    Ahead.y = 0;

    Ahead = glm::normalize(Ahead);
    
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::setObjState(std::vector<ObjState*> _objs)
{
    objects = _objs;
    // for(auto i = 0; i < _objs.size(); i++){
    //     objects.push_back(_objs[i]);
    // }
}

// check camera position to meet phisical restrict
void Camera::physicalPositionCheck()
{
    if (Position[1] < 0.5)
    {
        Position[1] = 0.5;
    }
    // cout << "old: " << Position_old.x << " " << Position_old.y << " " << Position_old.z << "   ";
    // cout << "new: " << Position.x << " " << Position.y << " " << Position.z << endl;

    if(ifCollide()){
        // cout << "collide" << endl;
        Position = Position_old;
    }
}

bool Camera::ifCollide()
{
    bool iscollide = false;
    for(auto i = 0; i < objects.size(); i++){
        switch (objects[i]->getType())
        {
        case ObjType::sphere :{
            iscollide |= ifCollideSphere(objects[i]);
            break;
        }
        case ObjType::cylinder :{
            iscollide |= ifCollideCylinder(objects[i]);
            break;
        }
        case ObjType::box :{
            iscollide |= ifCollideBox(objects[i]);
            break;
        }
        default:
            break;
        }
    }
    return iscollide;
}

bool Camera::ifCollideSphere(ObjState *sphere)
{
    bool iscollide = false;
    float r = (sphere->getSize().x)/2.0f;
    glm::vec3 diff = Position - sphere->getPos();
    float distance = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    if(distance < (r + Radius)*(r + Radius)){
        iscollide = true;
        cout << "walk into sphere" << endl;
    }
    return iscollide;
}

bool Camera::ifCollideCylinder(ObjState *cylinder)
{
    bool iscollide = false;
    float r = (cylinder->getSize().x)/2.0f;
    if(cylinder->getAngle() == 0.0f){
        float h = cylinder->getPos().y + (cylinder->getSize().y)/2.0f;
        glm::vec3 diff = Position - cylinder->getPos();
        float dist_xz = diff.x * diff.x + diff.z * diff.z;
        // cout << "dist_xz = " << dist_xz << endl;
        if((Position.y <= h && dist_xz <= (r + Radius)*(r + Radius))
        || (Position.y <= h + Radius && dist_xz <= r * r)
        // (y-h)^2 + (sqrt(dist)-r)^2 <= R^2
        || ((Position.y - h) * (Position.y - h) + (sqrt(dist_xz) - r) * (sqrt(dist_xz) - r) <= Radius * Radius)){
            iscollide = true;
            cout << "walk into cylinder" << endl;
        }
    }else{
        float front = cylinder->getPos().z + (cylinder->getSize().y)/2.0f;
        float back = cylinder->getPos().z - (cylinder->getSize().y)/2.0f;
        //cout << "front = " << front << " back = " << back << endl;
        glm::vec3 diff = Position - cylinder->getPos();
        float dist_xy = diff.x * diff.x + diff.y * diff.y;
        //cout << "dist_xy = " << dist_xy << endl;
        if((Position.z <= front && Position.z >= back && dist_xy <= (r + Radius)*(r + Radius))
        || (Position.z <= front + Radius && Position.z >= back - Radius && dist_xy <= r * r)
        || ((Position.z - front) * (Position.z - front) + (sqrt(dist_xy) - r) * (sqrt(dist_xy) - r) <= Radius * Radius)
        || ((Position.z - back) * (Position.z - back) + (sqrt(dist_xy) - r) * (sqrt(dist_xy) - r) <= Radius * Radius)){

            iscollide = true;
            cout << "walk into cylinder" << endl;
        }
    }
    return iscollide;
}

bool Camera::ifCollideBox(ObjState *box)
{
    bool iscollide = false;
    float sin_th = sin(glm::radians(box->getAngle()));
    float cos_th = cos(glm::radians(box->getAngle()));
    float boxpos_rot_x = box->getPos().x * cos_th - box->getPos().z * sin_th;
    float boxpos_rot_z = box->getPos().x * sin_th + box->getPos().z * cos_th;
    float mypos_rot_x = Position.x * cos_th - Position.z * sin_th;
    float mypos_rot_z = Position.x * sin_th + Position.z * cos_th;
    float x_left = boxpos_rot_x - box->getSize().x * 0.5f - Radius;
    float x_right = boxpos_rot_x + box->getSize().x * 0.5f + Radius;
    float z_front = boxpos_rot_z - box->getSize().z * 0.5f - Radius;
    float z_back = boxpos_rot_z + box->getSize().z * 0.5f + Radius;
    float h = box->getPos().y + box->getSize().y * 0.5f + Radius;
    
    if(Position.y <= h 
    && mypos_rot_x >= x_left && mypos_rot_x <= x_right
    && mypos_rot_z >= z_front && mypos_rot_z <= z_back){
        iscollide = true;
        cout << "walk into box" << endl;
    }
    return iscollide;
}