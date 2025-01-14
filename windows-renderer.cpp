#define UNICODE
#define _UNICODE
#include "windows-renderer.h"

namespace renderer
{
    std::vector<HBITMAP> renderQueue = {}; // pointers to graphics for rendering

    Renderer::Renderer() // constructor
    {

    }
    Renderer::~Renderer() // destructor
    {

    }

    std::array<unsigned short, 2> Renderer::getBitmapDimensions(HBITMAP hBitmap)
    {
        BITMAP bitmapInfo;

        // Zero out the BITMAP structure to avoid unexpected data
        ZeroMemory(&bitmapInfo, sizeof(BITMAP));

        // Retrieve the bitmap data
        if (hBitmap && GetObject(hBitmap, sizeof(BITMAP), &bitmapInfo)) 
        {
            // Successfully retrieved bitmap dimensions
            return std::array<unsigned short, 2>{(unsigned short)bitmapInfo.bmWidth, (unsigned short)bitmapInfo.bmHeight};
        }
        else 
        {
            // If GetObject failed, show an error message and return default 0, 0 dimensions
            DWORD errorCode = GetLastError();
            std::wstring errorMessage = L"Failed to retrieve bitmap data. Error code: ";
            errorMessage += std::to_wstring(errorCode);
            MessageBoxW(nullptr, errorMessage.c_str(), L"Error", MB_ICONERROR);
            
            // Return 0 dimensions if the function fails
            return {0, 0};
        }
    }

    LRESULT CALLBACK Renderer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    HWND windowHandle;

    int Renderer::initialiseWindow(std::array<unsigned short, 2> dimensions, std::array<unsigned short, 2> origin, const WCHAR* windowTitle) 
    {
        // Define and register the window class
        WNDCLASSEX wc = {0};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = Renderer::WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = L"WindowClass";
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

        if (!RegisterClassEx(&wc)) 
        {
            MessageBoxW(nullptr, L"Failed to register window class!", L"Error", MB_ICONERROR);
            return -1;
        }

        // Create the window
        windowHandle = CreateWindowExW(
            0, wc.lpszClassName, windowTitle, WS_OVERLAPPEDWINDOW,
            origin[0], origin[1], dimensions[0], dimensions[1],
            nullptr, nullptr, GetModuleHandle(nullptr), nullptr
        );

        if (!windowHandle) 
        {
            MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
            return -2;
        }

        // Show the window
        ShowWindow(windowHandle, SW_SHOW);
        UpdateWindow(windowHandle);

        return 0; // Return immediately after creating the window
    }
        
    HBITMAP Renderer::renderFrame(unsigned short frameWidth, unsigned short frameHeight) // Renders the frame 
    {
        // -- Prepare frame canvas -- //
        HBITMAP frame = CreateCompatibleBitmap(GetDC(windowHandle), frameWidth, frameHeight);
        HDC frameDC = CreateCompatibleDC(nullptr); 
        HBITMAP oldFrameBitmap = (HBITMAP)SelectObject(frameDC, frame); // Select frame bitmap into DC

        // Use BLENDFUNCTION to set alpha parameters
        BLENDFUNCTION blendFunc;
        blendFunc.BlendOp = AC_SRC_OVER; // Standard alpha blending
        blendFunc.BlendFlags = 0; // Must be 0
        blendFunc.SourceConstantAlpha = 255; // Fully opaque source
        blendFunc.AlphaFormat = AC_SRC_ALPHA; // Use alpha channel in the graphic(s)
        
        // -- Create device context for graphics -- //
        HDC graphicDC = CreateCompatibleDC(nullptr); // Single graphics DC for all graphics
        HBITMAP oldGraphicBitmap = nullptr; // Storage for previous graphic bitmap

        // -- Rendering!!!!! -- //
        for (HBITMAP graphic : renderQueue) // Go through all the graphics 
        {
            oldGraphicBitmap = (HBITMAP)SelectObject(graphicDC, graphic);
            std::array<unsigned short, 2> graphicDims = getBitmapDimensions(graphic); // Prepare the graphic's dimensions
            BitBlt(frameDC, 0, 0, graphicDims[0], graphicDims[1], graphicDC, 0, 0, SRCCOPY); // Copy the graphic onto the frame

        }

        SelectObject(frameDC, oldFrameBitmap); // Restore original frame bitmap in the DC
        DeleteObject(oldFrameBitmap);
        DeleteObject(oldGraphicBitmap);
        DeleteDC(frameDC);
        DeleteDC(graphicDC);

        return frame;
    }

    void Renderer::presentFrame(HBITMAP frame) // Presents the frame to the window
    {
        // -- Prepare frame & window DCs -- //
        HDC hdcWindow = GetDC(windowHandle); // Get device context for window
        HDC hdcMem = CreateCompatibleDC(hdcWindow); // Compatible device context for the frame
        HBITMAP oldFrame = (HBITMAP)SelectObject(hdcMem, frame); // Select the frame into the compatible DC

        std::array<unsigned short, 2> frameDims = getBitmapDimensions(frame); // Prepare frame dimensions

        // -- Prepare the blend function for alpha blending -- //
        BLENDFUNCTION blendFunc;
        blendFunc.BlendOp = AC_SRC_OVER; // Standard alpha blending
        blendFunc.BlendFlags = 0; // Must be 0
        blendFunc.SourceConstantAlpha = 255; // Fully opaque source (can adjust for transparency)
        blendFunc.AlphaFormat = AC_SRC_ALPHA; // Use alpha channel
        
        // -- PRESENTATION -- //
        BitBlt(hdcWindow, 0, 0, frameDims[0], frameDims[1], hdcMem, 0, 0, SRCCOPY); // Copy the frame to the window's DC

        // -- Clean up -- //
        SelectObject(hdcMem, oldFrame);
        DeleteDC(hdcMem);
        ReleaseDC(windowHandle, hdcWindow);
    }

    void Renderer::presentTestBitmap() 
    {
        HDC hdcWindow = GetDC(windowHandle);
        HDC hdcMem = CreateCompatibleDC(hdcWindow);

        // Create a test bitmap (solid red colour)
        HBITMAP testBitmap = CreateCompatibleBitmap(hdcWindow, 512, 512);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, testBitmap);

        RECT rect = {0, 0, 512, 512};
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(hdcMem, &rect, redBrush);

        // Present the bitmap
        BitBlt(hdcWindow, 0, 0, 512, 512, hdcMem, 0, 0, SRCCOPY);

        // Clean up
        SelectObject(hdcMem, oldBitmap);
        DeleteObject(testBitmap);
        DeleteObject(redBrush);
        DeleteDC(hdcMem);
        ReleaseDC(windowHandle, hdcWindow);
    }


    void Renderer::setPixelColour(unsigned short x, unsigned short y, std::array<unsigned char, 3> colour) // Sets an individual pixel's colour
    {
        // TODO
    }

    void Renderer::appendBitmapToQueue(HBITMAP bitmap) // adds a bitmap to renderQueue
    {
        renderQueue.push_back(bitmap);
    }

    HBITMAP Renderer::imageToHBITMAP(const WCHAR* imagePath, unsigned int width, unsigned int height) 
    {
        // Use LoadImage to load the bitmap file
        HBITMAP hBitmap = (HBITMAP)LoadImageW(
            nullptr, // No instance handle required when loading from a file
            imagePath,        
            IMAGE_BITMAP, // Specifies that the image type is a bitmap
            width, height, // Width and height are 0 by default (check the header file) so the function will use the .bmp's dimensions
            LR_LOADFROMFILE | LR_CREATEDIBSECTION // Load from file and create DIB section for flexibility
        );

        if (!hBitmap)
        {
            // Display the error message with the path
            std::wstring errorMessage = L"Failed to load file at: ";
            errorMessage += imagePath;
            MessageBoxW(nullptr, errorMessage.c_str(), L"Error", MB_ICONERROR);
        }
        return hBitmap;
    }
}

/*
COMPILE COMMANDS (run BOTH in THIS ORDER):
g++ -c windows-renderer.cpp -o windows-renderer.o -lgdi32 -lmsimg32 
ar rcs libwindows-renderer.a windows-renderer.o
*/