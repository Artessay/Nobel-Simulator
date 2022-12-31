#include "core.h"
#include <iostream>

int star();

int main()
{
    std::cout << "Hello ZJUer" << std::endl;
    
    try
    {
        // star();
        core();
    }
    catch(...)
    {
        std::cerr << "CG core error" << '\n';
    }
    
    std::cout << "Goodbye ZJUer" << std::endl;

    return 0;
}