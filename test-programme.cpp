//#include <iostream> // Comment out when not debugging
#include <cmath>

#include "windows-renderer.h" // COMPILE windows-renderer.cpp SEPERATELY

renderer::Renderer renderingEngine = renderer::Renderer(); 
std::array<unsigned short, 2> winDims = {0x400,0x400};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) // Winmain makes the console not open
{
    renderingEngine.initialiseWindow(winDims, {0, 0}); 

    HBITMAP prepImage = renderingEngine.imageToHBITMAP(L"blackbuck.bmp");
    //renderingEngine.appendBitmapToQueue(prepImage);

    for (int i = 0; i < winDims[1]; i++)
    {
        for (int ii = 0; ii < winDims[0]; ii++)
        {
            std::array<unsigned char, 3> c = (bool)(winDims[1]-i < pow(ii-0x200,2)/300 + 300) ? std::array<unsigned char, 3>{0xFF, 0x00, 0x00} : std::array<unsigned char, 3>{0x00, 0x00, 0xFF};
            renderingEngine.setPixelColour(ii,i,c);
        }
    }

    while (renderingEngine.checkAndSendMessage()) // while the window is open
    {
        // Render and present frames
        renderingEngine.presentFrame(renderingEngine.renderFrame());
    }

    return 0;
}


// COMPILE COMMAND:
// g++ test-programme.cpp -o test-programme -L. -lwindows-renderer -lgdi32 -lmsimg32 -mwindows

// Here's a gun in case it goes rogue:
// taskkill /IM test-programme.exe /F