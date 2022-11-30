#include "core.h"
#include <iostream>

int main()
{
    std::cout << "Hello ZJUer" << std::endl;
    
    try
    {
        core();
        // loadModel();
    }
    catch(...)
    {
        std::cerr << "CG core error" << '\n';
    }
    
    std::cout << "Goodbye ZJUer" << std::endl;

    return 0;
}