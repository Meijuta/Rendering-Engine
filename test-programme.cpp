#include <iostream>
#include "windows-renderer.cpp"

renderer::Renderer renderingEngine = renderer::Renderer(); 
std::array<unsigned short, 2> winDims = {0x200,0x180};

int main()
{
    renderingEngine.initialiseWindow(winDims,{0,0});
    
    return 0;
}