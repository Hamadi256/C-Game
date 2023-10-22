#include <windows.h>
#include <stdint.h>

#define local_function static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

global_variable bool running = true;
global_variable void *BitmapMemory;


local_function void displayPixel(int width, int height, int bytesPerPixel) {
    int Pitch = width * bytesPerPixel;
    uint8* Row = (uint8*)BitmapMemory;

    for (int y = 0; y < height; ++y) {
        uint8* Pixel = (uint8*)Row;
        for (int x = 0; x < width; ++x) {

            *Pixel = 0;
            Pixel++;

            *Pixel = 0;
            Pixel++;

            *Pixel = 255;
            Pixel++;

            *Pixel = 0;
            Pixel++;
        };
        Row += Pitch;
    };
};

local_function void BitmapResize(int width, int height) {
    if (BitmapMemory) {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    int bytesPerPixel = 4;
    int BitmapSize = (width * height) * bytesPerPixel;
    BitmapMemory = VirtualAlloc(
        0,
        BitmapSize,
        MEM_COMMIT,
        PAGE_READWRITE);
    if (!BitmapMemory) {
        MessageBoxA(0, "VirtualAlloc failed", "Shit", 0);
    };

    displayPixel(width, height, bytesPerPixel);

    
};

local_function void BitmapRepaint(HDC handleDeviceContext, int width, int height) {

    BITMAPINFO myBitmapInfo;
    myBitmapInfo.bmiHeader.biSize = sizeof(myBitmapInfo.bmiHeader);
    myBitmapInfo.bmiHeader.biWidth = width;
    myBitmapInfo.bmiHeader.biHeight = -height;
    myBitmapInfo.bmiHeader.biPlanes = 1;
    myBitmapInfo.bmiHeader.biBitCount = 32;
    myBitmapInfo.bmiHeader.biCompression = BI_RGB;

    int outcome = StretchDIBits(
        handleDeviceContext,
        0,
        0,
        width,
        height,
        0,
        0,
        width,
        height,
        BitmapMemory,
        &myBitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);

    if (!outcome) {
        MessageBoxA(0, "StretchDIBits failed", "shit", 0);
    };
};

LRESULT Wndproc(
    HWND Window,
    UINT Message,
    WPARAM unnamedParam3,
    LPARAM unnamedParam4)
{

    switch (Message) {
        case WM_DESTROY: 
            running = false;
            break;

        case WM_CLOSE:
            running = false;
            break;

        case WM_SIZE: {
            RECT clientRect;
            GetClientRect(Window, &clientRect);
            int Width = clientRect.right - clientRect.left;
            int Height = clientRect.bottom - clientRect.top;
            BitmapResize(Width, Height);
        }
            break;
           

        case WM_PAINT: 
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);

            RECT myRect;
            GetClientRect(Window, &myRect);
            int Width = myRect.right - myRect.left;
            int Height = myRect.bottom - myRect.top;
            BitmapRepaint(DeviceContext, Width, Height);
            EndPaint(Window, &Paint);
        };
        break;

        default: 
            return DefWindowProcA(Window, Message, unnamedParam3, unnamedParam4);
            break;
    }
}

int WINAPI WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow) 
{

    WNDCLASSEXA windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXA);
    windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc = Wndproc;
    windowClass.hInstance = Instance;
    windowClass.lpszClassName = "Game";


    if (RegisterClassExA(&windowClass)) {
        HWND windowHandle = CreateWindowExA(
            0,
            windowClass.lpszClassName,
            "GameV1",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance,
            0);

        if (windowHandle) {
            MSG Message;
            while (running) {
                while (PeekMessageA(
                    &Message,
                    0,
                    0,
                    0,
                    PM_REMOVE))
                {
                    if (Message.message == WM_QUIT) {
                        running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                   
            }
             
        }
        else {
            MessageBoxA(0, "windowHandle failure", "Bruh", 0);
            //Add error handling;
        }
    }
    else {
        //Error Handling
        MessageBoxA(0, "RegisterClass failure", "Bruh", 0);
    }
        
};
