#ifndef _CUBE_MAP_H_
#define _CUBE_MAP_H_

#include "Cube.h"
#include <vector>
#include <string>

/**
 * @brief Cube Map for background texture
 * 
 */
class CubeMap: public Cube
{
    public:
        CubeMap(const std::vector<std::string>& textures);

        virtual void render();

    private:
        unsigned int textureID;
};

#endif