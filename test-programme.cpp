//#include <iostream> // Comment out when not debugging
#include "windows-renderer.h" // COMPILE windows-renderer.cpp SEPERATELY

renderer::Renderer renderingEngine = renderer::Renderer(); 
std::array<unsigned short, 2> winDims = {0x200,0x200};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) // Winmain makes the console not open
{
    renderingEngine.initialiseWindow(winDims, {0, 0}); 

    HBITMAP prepImage = renderingEngine.imageToHBITMAP(L"blackbuck.bmp");
    renderingEngine.appendBitmapToQueue(prepImage);

    while (renderingEngine.checkAndSendMessage()) // while the window is open
    {
        // Render and present frames
        renderingEngine.presentFrame(renderingEngine.renderFrame(winDims[0], winDims[1]));
    }

    return 0;
}


// COMPILE COMMAND:
// g++ test-programme.cpp -o test-programme -L. -lwindows-renderer -lgdi32 -lmsimg32 -mwindows

// Here's a gun in case it goes rogue:
// taskkill /IM test-programme.exe /F