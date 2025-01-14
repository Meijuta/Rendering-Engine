#ifndef WINDOWS_RENDERER_H
#define WINDOWS_RENDERER_H
#include <windows.h>
#include <wingdi.h> // ;-;
#include <vector>
#include <array>
#include <string>

#include <iostream> // comment out when not debugging

namespace renderer 
{
    class Renderer
    {
        private:
            std::vector<HBITMAP> renderQueue; // pointers to graphics for rendering
            HWND windowHandle;
            static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        public:
            Renderer(); // Constructor
            ~Renderer(); // Destructor
            std::array<unsigned short, 2> getBitmapDimensions(HBITMAP hBitmap);
            int initialiseWindow(std::array<unsigned short, 2> dimensions, std::array<unsigned short, 2> origin, const WCHAR* windowTitle = L"My Renderer Window"); // creates the window being rendered to.
            HBITMAP renderFrame(unsigned short frameWidth, unsigned short frameHeight); // Renders the frame
            void presentFrame(HBITMAP frame); // Presents the frame to the window
            void presentTestBitmap();
            void setPixelColour(unsigned short x, unsigned short y, std::array<unsigned char, 3> colour); // sets an individual pixel's colour
            void appendBitmapToQueue(HBITMAP bitmap); // Adds a bitmap to renderQueue
            HBITMAP imageToHBITMAP(const WCHAR* imagePath, unsigned int width=0, unsigned int height=0); // Takes a bmp file from a given path and returns it as an HBITMAP
    };
}

#endif