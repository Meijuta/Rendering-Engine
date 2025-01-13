#ifndef WINDOWS_RENDERER_H
#define WINDOWS_RENDERER_H
#include <windows.h>
#include <vector>
#include <array>

namespace renderer 
{
    class Renderer
    {
        private:
            std::vector<int*> renderQueue; // pointers to graphics for rendering
            static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        public:
            Renderer(); // constructor
            ~Renderer(); // destructor
            int initialiseWindow(std::array<unsigned short, 2> dimensions, std::array<unsigned short, 2> origin, const WCHAR* windowTitle = L"My Renderer Window"); // creates the window being rendered to.
            void renderFrame(); // renders the frame
            void presentFrame(); // presents the frame to the window
            void setPixelColour(unsigned short x, unsigned short y, std::array<unsigned char, 3> colour); // sets an individual pixel's colour
    };
}

#endif