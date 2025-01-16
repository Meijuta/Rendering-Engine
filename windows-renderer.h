#ifndef WINDOWS_RENDERER_H
#define WINDOWS_RENDERER_H
#include <windows.h>
#include <wingdi.h> // ;-;
#include <vector>
#include <array>
#include <string>

//#include <iostream> // comment out when not debugging

namespace renderer 
{
    class Renderer
    {
        private:
            std::vector<HBITMAP> renderQueue; // pointers to graphics for rendering
            HWND windowHandle;
            std::array<unsigned short, 2> windowDimensions;
            HBITMAP pixelEdits;
            static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Handles messages to the window
        public:
            Renderer(); // Constructor
            ~Renderer(); // Destructor
            HBITMAP createTransparentBitmap(unsigned short width, unsigned short height);
            HBITMAP cloneBitmap(HBITMAP source);
            std::array<unsigned short, 2> getBitmapDimensions(HBITMAP hBitmap);
            int initialiseWindow(std::array<unsigned short, 2> dimensions, std::array<unsigned short, 2> origin, const WCHAR* windowTitle = L"My Renderer Window"); // Creates the window being rendered to
            bool checkAndSendMessage(); // Checks if there is a message to the window and if there is one it sends it to the window. If the message is to close it returns false
            HBITMAP renderFrame(); // Renders the frame
            void presentFrame(HBITMAP frame); // Presents the frame to the window
            void presentTestBitmap();
            bool setPixelColour(unsigned short x, unsigned short y, std::array<unsigned char, 3> colour, unsigned char opacity = 0xFF); // sets an individual pixel's colour
            void clearRenderQueue();
            void appendBitmapToQueue(HBITMAP bitmap); // Adds a bitmap to renderQueue
            HBITMAP imageToHBITMAP(const WCHAR* imagePath, unsigned int width=0, unsigned int height=0); // Takes a bmp file from a given path and returns it as an HBITMAP
            void drawRectangle(unsigned short x, unsigned short y, unsigned short width, unsigned short height, std::array<unsigned char, 3> colour, unsigned char opacity = 0xFF); // I REALLY hope this doesnt need explaining
    };
}

#endif