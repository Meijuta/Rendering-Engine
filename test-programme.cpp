#include <iostream>
#include "windows-renderer.h" // COMPILE windows-renderer.cpp SEPERATELY

renderer::Renderer renderingEngine = renderer::Renderer(); 
std::array<unsigned short, 2> winDims = {0x200,0x180};

int main() 
{
    renderingEngine.initialiseWindow(winDims, {0, 0}); 

    HBITMAP prepImage = renderingEngine.imageToHBITMAP(L"blackbuck.bmp");
    renderingEngine.appendBitmapToQueue(prepImage);
    HBITMAP frame;

    MSG msg; // For processing Windows messages

    while (true) {
        // Handle window messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Render and present frames
        frame = renderingEngine.renderFrame(winDims[0], winDims[1]);
        renderingEngine.presentFrame(frame);
    }

    return 0;
}


// COMPILE COMMAND:
// g++ test-programme.cpp -o test-programme -L. -lwindows-renderer -lgdi32 -lmsimg32