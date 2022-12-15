#ifndef _BOMB_H_
#define _BOMB_H_

#include "Shader.h"
#include "Sphere.h"

#include <set>
#include <glm/glm.hpp>

#define MAX_BOMBS 10

class Bomb
{
    public:

        static int getBombNumber();
        static glm::vec3 getBombPosition(int index);
        
        static bool placeBomb(glm::vec3 bombPosition, glm::vec3 bombFront);
        
        static void draw(Shader& shader);
    
    protected:
        const static float bomb_velocity;
        const static float bomb_initial_height;
        static Sphere shape;

        Bomb(glm::vec3 bombPosition, glm::vec3 bombFront);

        static int bomb_number;    // bomb number
        static std::set<Bomb*> bombSet;
        static glm::vec3 bombPositions[MAX_BOMBS];

        float start_time;
        glm::vec3 position;
        glm::vec3 front;

    private:
        unsigned int bomb_ID;
        
        bool operator==( const Bomb& rhs ) const
        {
            return bomb_ID == rhs.bomb_ID;
        }

        bool operator!=( const Bomb& rhs ) const
        {
            return !(*this == rhs);
        }

        bool operator<( const Bomb& rhs ) const
        {
            return bomb_ID < rhs.bomb_ID;
        }

        bool operator>( const Bomb& rhs ) const
        {
            return rhs < *this;
        }

        bool operator<=( const Bomb& rhs ) const
        {
            return !(rhs < *this);
        }

        bool operator>=( const Bomb& rhs ) const
        {
            return !(*this < rhs);
        }
};

#endif