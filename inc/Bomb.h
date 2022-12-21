#ifndef _BOMB_H_
#define _BOMB_H_

#include "Shader.h"
#include "Sphere.h"
#include "Texture.h"
#include <set>
#include <glm/glm.hpp>
#include <vector>

#define MAX_BOMBS 10

class Bomb
{
    public:

        glm::vec3 getPosition() const { return position; }

        void explode(Shader& shader,glm::vec3 position);

        static std::vector <glm::vec3> boomed_bombSet;
        static std::set<Bomb*> bombSet;

        static int getBombNumber();
        
        static bool placeBomb(glm::vec3 bombPosition, glm::vec3 bombFront);
        
        static void draw(Shader& Shader);

        static void drawRuin(Shader& shader);
    
    protected:
        const static float bomb_velocity;

        Bomb(glm::vec3 bombPosition, glm::vec3 bombFront);

        virtual ~Bomb();

        static int bomb_number;    // bomb number
        // static glm::vec3 bombPositions[MAX_BOMBS];

        float start_time;
        glm::vec3 start_position;
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