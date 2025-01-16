#define UNICODE
#define _UNICODE
#include "windows-renderer.h"

namespace renderer
{
    std::vector<HBITMAP> renderQueue = {}; // pointers to graphics for rendering
    std::array<unsigned short, 2> windowDimensions;
    HWND windowHandle;
    HBITMAP pixelEdits;

    Renderer::Renderer() // constructor
    {

    }
    Renderer::~Renderer() // destructor
    {
        if (pixelEdits) 
        {
            DeleteObject(pixelEdits);
        }
    }



    HBITMAP Renderer::createTransparentBitmap(unsigned short width, unsigned short height)
    {
        // Define the bitmap info
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height; // Negative for top-down DIB
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32; // 32 bits for ARGB (alpha channel + RGB)
        bmi.bmiHeader.biCompression = BI_RGB;

        // Create a DIB section
        void* bits = nullptr;
        HBITMAP hBitmap = CreateDIBSection
        (
            nullptr, // Optional HDC, can be null
            &bmi, // Bitmap information
            DIB_RGB_COLORS, // Colour table type
            &bits, // Pointer to bitmap pixel data
            nullptr, // Optional file mapping
            0 // Offset in the file mapping
        );

        if (!hBitmap || !bits)
        {
            return nullptr;
        }
        // Fill the bitmap with the specified colour
        UINT32* pixels = static_cast<UINT32*>(bits);
        for (int i = 0; i < width * height; ++i)
        {
            pixels[i] = 0x00000000; // All pixels are transparent! 
        }

        return hBitmap;
    }



    HBITMAP Renderer::cloneBitmap(HBITMAP source)
    {
        // Retrieve source bitmap dimensions
        std::array<unsigned short, 2> dims = getBitmapDimensions(source);

        // Create a compatible bitmap
        HDC screenDC = GetDC(nullptr);
        HBITMAP clone = CreateCompatibleBitmap(screenDC, dims[0], dims[1]);
        ReleaseDC(nullptr, screenDC);

        if (!clone) 
        {
            return nullptr;
        }

        // Copy pixel data from source to clone
        HDC sourceDC = CreateCompatibleDC(nullptr);
        HDC cloneDC = CreateCompatibleDC(nullptr);

        HBITMAP oldSourceBitmap = (HBITMAP)SelectObject(sourceDC, source);
        HBITMAP oldCloneBitmap = (HBITMAP)SelectObject(cloneDC, clone);

        BitBlt(cloneDC, 0, 0, dims[0], dims[1], sourceDC, 0, 0, SRCCOPY);

        SelectObject(sourceDC, oldSourceBitmap);
        SelectObject(cloneDC, oldCloneBitmap);

        DeleteDC(sourceDC);
        DeleteDC(cloneDC);

        return clone;
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



    LRESULT CALLBACK Renderer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // Handles messages to the window
    {
        switch (uMsg)
        {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }



    int Renderer::initialiseWindow(std::array<unsigned short, 2> dimensions, std::array<unsigned short, 2> origin, const WCHAR* windowTitle) // Creates the window
    {
        windowDimensions = dimensions;
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
            origin[0], origin[1], windowDimensions[0], windowDimensions[1],
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

        pixelEdits = createTransparentBitmap(windowDimensions[0],windowDimensions[1]);
        if (!pixelEdits) 
        {
            MessageBoxW(nullptr, L"Failed to create pixel edits bitmap!", L"Error", MB_ICONERROR);
            return -3; 
        }

        return 0; // Return immediately after creating the window
    }



    bool Renderer::checkAndSendMessage() // Checks if there is a message to the window and if there is one it sends it to the window. If the message is to close it returns false
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
    }


        
    HBITMAP Renderer::renderFrame() 
    {
        // Clone pixelEdits and add it to the renderQueue
        HBITMAP clonedEdits = cloneBitmap(pixelEdits);
        if (clonedEdits) 
        {
            renderQueue.push_back(clonedEdits); // Push the clone, not the original
        }

        // Prepare the frame canvas
        HBITMAP frame = CreateCompatibleBitmap(GetDC(windowHandle), windowDimensions[0], windowDimensions[1]);
        HDC frameDC = CreateCompatibleDC(nullptr); 
        HBITMAP oldFrameBitmap = (HBITMAP)SelectObject(frameDC, frame);

        // Use BLENDFUNCTION for alpha blending
        BLENDFUNCTION blendFunc;
        blendFunc.BlendOp = AC_SRC_OVER;
        blendFunc.BlendFlags = 0;
        blendFunc.SourceConstantAlpha = 0xFF;
        blendFunc.AlphaFormat = AC_SRC_ALPHA;

        // Create a graphics device context
        HDC graphicDC = CreateCompatibleDC(nullptr);
        HBITMAP oldGraphicBitmap = nullptr;

        // Render each graphic in the queue
        for (HBITMAP graphic : renderQueue) 
        {
            oldGraphicBitmap = (HBITMAP)SelectObject(graphicDC, graphic);
            std::array<unsigned short, 2> graphicDims = getBitmapDimensions(graphic);
            AlphaBlend

            (
            frameDC, 0, 0, graphicDims[0], graphicDims[1], // Destination rectangle
            graphicDC, 0, 0, graphicDims[0], graphicDims[1], // Source rectangle
            blendFunc
            );

            SelectObject(graphicDC, oldGraphicBitmap); // Deselect graphic from DC
        }

        // Clean up
        SelectObject(frameDC, oldFrameBitmap);
        DeleteDC(frameDC);
        DeleteDC(graphicDC);

        // Delete used bitmaps in the queue and clear it
        for (HBITMAP graphic : renderQueue) 
        {
            DeleteObject(graphic);
        }
        renderQueue.clear();

        return frame;
    }



    void Renderer::presentFrame(HBITMAP frame) // Presents the frame to the window
    {
        // -- Prepare frame & window DCs -- //
        HDC hdcWindow = GetDC(windowHandle); // Get device context for window
        HDC hdcFrame = CreateCompatibleDC(hdcWindow); // Compatible device context for the frame
        HBITMAP oldFrame = (HBITMAP)SelectObject(hdcFrame, frame); // Select the frame into the compatible DC

        std::array<unsigned short, 2> frameDims = getBitmapDimensions(frame); // Prepare frame dimensions

        // -- Prepare the blend function for alpha blending -- //
        BLENDFUNCTION blendFunc;
        blendFunc.BlendOp = AC_SRC_OVER; // Standard alpha blending
        blendFunc.BlendFlags = 0; // Must be 0
        blendFunc.SourceConstantAlpha = 255; // Fully opaque source (can adjust for transparency)
        blendFunc.AlphaFormat = AC_SRC_ALPHA; // Use alpha channel
        
        // -- PRESENTATION -- //
        AlphaBlend
        (
            hdcWindow, 0, 0, windowDimensions[0], windowDimensions[1],
            hdcFrame, 0, 0, frameDims[0], frameDims[1], 
            blendFunc
        );

        // -- Clean up -- //
        SelectObject(hdcFrame, oldFrame);
        DeleteDC(hdcFrame);
        ReleaseDC(windowHandle, hdcWindow);
    }



    void Renderer::presentTestBitmap() 
    {
        HDC hdcWindow = GetDC(windowHandle);
        HDC hdcMem = CreateCompatibleDC(hdcWindow);

        // Create a test bitmap (solid red colour)
        HBITMAP testBitmap = CreateCompatibleBitmap(hdcWindow, 0x400, 0x400);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, testBitmap);

        RECT rect = {0, 0, 0x200, 0x200};
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(hdcMem, &rect, redBrush);

        // Present the bitmap
        BitBlt(hdcWindow, 0, 0, 0x200, 0x200, hdcMem, 0, 0, SRCCOPY);

        // Clean up
        SelectObject(hdcMem, oldBitmap);
        DeleteObject(testBitmap);
        DeleteObject(redBrush);
        DeleteDC(hdcMem);
        ReleaseDC(windowHandle, hdcWindow);
    }



    bool Renderer::setPixelColour(unsigned short x, unsigned short y, std::array<unsigned char, 3> colour, unsigned char opacity)
    {
        // Ensure coordinates are within the bounds of the bitmap
        if (x >= windowDimensions[0] || y >= windowDimensions[1])
        {
            return false; // Out-of-bounds access
        }

        // Retrieve bitmap information
        DIBSECTION dibSection = {};
        if (GetObject(pixelEdits, sizeof(DIBSECTION), &dibSection) != sizeof(DIBSECTION))
        {
            return false; // Failed to retrieve bitmap data
        }

        // Get a pointer to the pixel data
        UINT32* pixels = static_cast<UINT32*>(dibSection.dsBm.bmBits);
        if (!pixels)
        {
            return false; // Failed to get pixel data
        }

        // Calculate the ARGB value
        UINT32 argb = (static_cast<UINT32>(opacity) << 24) | // Alpha
                    (static_cast<UINT32>(colour[0]) << 16) | // Red
                    (static_cast<UINT32>(colour[1]) << 8) | // Green
                    static_cast<UINT32>(colour[2]); // Blue

        // Set the pixel colour
        pixels[y * windowDimensions[0] + x] = argb;

        return true; // Success
    }



    void Renderer::clearRenderQueue()
    {
        renderQueue = {};
    }



    void Renderer::appendBitmapToQueue(HBITMAP bitmap) // adds a bitmap to renderQueue
    {
        renderQueue.push_back(bitmap);
    }



    HBITMAP Renderer::imageToHBITMAP(const WCHAR* imagePath, unsigned int width, unsigned int height) // TODO: force images to have an alpha channel
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



    void Renderer::drawRectangle(unsigned short x, unsigned short y, unsigned short width, unsigned short height, std::array<unsigned char, 3> colour, unsigned char opacity)
    {
        for (unsigned int i = 0; i < height; i++)
        {   
            for (unsigned int ii = 0; ii < width; ii++)
            {
                setPixelColour
                (
                    x + ii,
                    y + i,
                    colour,
                    opacity
                );
            }
        }
    }
}

/*
COMPILE COMMANDS (run BOTH in THIS ORDER):
g++ -c windows-renderer.cpp -o windows-renderer.o -lgdi32 -lmsimg32 
ar rcs libwindows-renderer.a windows-renderer.o
*/