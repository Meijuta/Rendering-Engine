#define UNICODE
#define _UNICODE
#include "windows-renderer.h"

namespace renderer
{
    Renderer::Renderer() // constructor
    {

    }
    Renderer::~Renderer() // destructor
    {

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

    int Renderer::initialiseWindow(std::array<unsigned short, 2> dimensions, std::array<unsigned short, 2> origin, const WCHAR* windowTitle) // creates the window being rendered to. returns an int on closing conveying how it closed
    {
        // -- Define and register a window class -- //
        WNDCLASSEX wc = { 0 /*Zero or nullptr is the default value for each field of the structure*/ };

        wc.cbSize = sizeof(WNDCLASSEX); // Set the size of the structure
        wc.style = CS_HREDRAW | CS_VREDRAW; // Class style (e.g., redraw on horizontal/vertical changes)
        wc.lpfnWndProc = Renderer::WindowProc; // Pointer to the window procedure
        wc.cbClsExtra = 0; // Extra bytes for the class
        wc.cbWndExtra = 0; // Extra bytes for the window
        wc.hInstance = GetModuleHandle(nullptr); // Handle to the instance of the application
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // Class icon
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Class cursor
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Background brush
        wc.lpszMenuName = nullptr; // Menu resource name
        wc.lpszClassName = L"WindowClass"; // Class name
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION); // Small icon for the class

        if (!RegisterClassEx(&wc)) // Register the window class
        {
            MessageBoxW(nullptr, L"Failed to register window class!", L"Error", MB_ICONERROR);
            return -1; 
        }

        // -- Create a window with CreateWindowEx -- //
        HWND hwnd = CreateWindowExW
        (
            0, // No window styling
            wc.lpszClassName, 
            windowTitle, // Window title
            WS_OVERLAPPEDWINDOW, // Standard window
            origin[0], 
            origin[1], 
            dimensions[0], 
            dimensions[1], 
            nullptr, // No parent window
            nullptr, // No Menu
            GetModuleHandle(nullptr), // Application instance handle
            nullptr // Additional data
        );

        if (!hwnd) {
            MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_ICONERROR);
            return -2;
        }

        // -- Show and update the window -- //
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        // -- Enter the message loop -- //  
        MSG msg; // Define a variable to store the message
        while (GetMessage(&msg, nullptr, 0, 0)) {
            // Translate message (keyboard input)
            TranslateMessage(&msg);
            DispatchMessage(&msg); // Dispatch to windowProc
        }

        // -- Return the exit code -- //
        return static_cast<int>(msg.wParam); // Return the exit code from the message loop
    }
    
    void Renderer::renderFrame() // renders the frame TODO: return a bitmap of the rendered frame
    {
        for (int* graphic : renderQueue)
        {
            // TODO: render the graphic to the frame
        }
    }
    void Renderer::presentFrame() // presents the frame to the window
    {

    }

    void Renderer::setPixelColour(unsigned short x, unsigned short y, std::array<unsigned char, 3> colour) // sets an individual pixel's colour
    {

    }
}