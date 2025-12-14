#include "MainWindow.h"
#include "HardwareInfo.h"
#include <windows.h>
#include <shellapi.h>
#include <gdiplus.h>
using namespace Gdiplus;
// Link instruction note: when compiling with g++ add -lgdiplus

// Color Palette
#define COLOR_BG_MAIN RGB(3, 41, 68)           // #032944
#define COLOR_BTN_ACTIVE RGB(7, 94, 156)      // #075E9C
#define COLOR_BG_SECONDARY RGB(34, 36, 37)   // #222425
#define COLOR_TEXT_PRIMARY RGB(12, 13, 13)    // #0C0D0D
#define COLOR_TEXT_SECONDARY RGB(146, 150, 155) // #92969B

// Use standard Windows control color messages
#ifndef WM_CTLCOLOREDIT
#define WM_CTLCOLOREDIT 0x0133
#endif
#ifndef WM_CTLCOLORBTN
#define WM_CTLCOLORBTN 0x0135
#endif
#ifndef WM_CTLCOLORSTATIC
#define WM_CTLCOLORSTATIC 0x0138
#endif

static ULONG_PTR gdiPlusToken = 0;
// Icon background color
#define COLOR_ICON_BG RGB(7, 94, 156)

MainWindow::MainWindow(HINSTANCE hInst) : hInstance(hInst), hwnd(NULL),
    cpuButton(NULL), ramButton(NULL), diskButton(NULL), gpuButton(NULL), osButton(NULL), outputBox(NULL), infoHeader(NULL),
    cpuBmp(NULL), ramBmp(NULL), diskBmp(NULL), gpuBmp(NULL), osBmp(NULL),
    cpuIcon(NULL), ramIcon(NULL), diskIcon(NULL), gpuIcon(NULL), osIcon(NULL),
    btnBrush(NULL), iconBgBrush(NULL), headerBrush(NULL)
{}

void MainWindow::Show(int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"HardwareInspector";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = MainWindow::WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(COLOR_BG_MAIN);
    wc.hIcon = (HICON)LoadImageW(NULL, L"Resources\\Icons\\main.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    RegisterClassW(&wc);

    // Initialize GDI+ to load PNG icons
    if (gdiPlusToken == 0)
    {
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&gdiPlusToken, &gdiplusStartupInput, NULL);
    }

    hwnd = CreateWindowExW(0, CLASS_NAME, L"Inspecteur Matériel",
                           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
                           NULL, NULL, hInstance, this);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static MainWindow* self = nullptr;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        self = (MainWindow*)cs->lpCreateParams;
    }

    if (!self)
        return DefWindowProcW(hwnd, msg, wParam, lParam);

    switch (msg)
    {
        case WM_CREATE: {
            // Create brushes once
            self->btnBrush = CreateSolidBrush(COLOR_BTN_ACTIVE);
            self->iconBgBrush = CreateSolidBrush(COLOR_ICON_BG);
            self->headerBrush = CreateSolidBrush(COLOR_BG_MAIN);

            // Header
            self->infoHeader = CreateWindowW(L"STATIC", L"INSPECTEUR MATERIEL - Cliquez sur un bouton pour voir les details", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                            0, 0, 800, 30, hwnd, NULL, self->hInstance, NULL);
            SendMessageW(self->infoHeader, WM_SETFONT, (WPARAM)CreateFontW(14, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI"), TRUE);

            // Create icon statics (40x40) and larger buttons (text only) beside them
            int ix = 10, iy = 40, iw = 40, ih = 40, bx = 60, bw = 180, bh = 40, spacing = 50;
            self->cpuIcon = CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
                                          ix, iy, iw, ih, hwnd, NULL, self->hInstance, NULL);
            self->cpuButton = CreateWindowW(L"BUTTON", L"CPU", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT,
                                           bx, iy, bw, bh, hwnd, (HMENU)1, self->hInstance, NULL);

            self->ramIcon = CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
                                          ix, iy + spacing, iw, ih, hwnd, NULL, self->hInstance, NULL);
            self->ramButton = CreateWindowW(L"BUTTON", L"RAM", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT,
                                           bx, iy + spacing, bw, bh, hwnd, (HMENU)2, self->hInstance, NULL);

            self->diskIcon = CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
                                           ix, iy + spacing*2, iw, ih, hwnd, NULL, self->hInstance, NULL);
            self->diskButton = CreateWindowW(L"BUTTON", L"DISQUES", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT,
                                            bx, iy + spacing*2, bw, bh, hwnd, (HMENU)3, self->hInstance, NULL);

            self->gpuIcon = CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
                                          ix, iy + spacing*3, iw, ih, hwnd, NULL, self->hInstance, NULL);
            self->gpuButton = CreateWindowW(L"BUTTON", L"GPU", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT,
                                           bx, iy + spacing*3, bw, bh, hwnd, (HMENU)4, self->hInstance, NULL);

            self->osIcon = CreateWindowW(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
                                         ix, iy + spacing*4, iw, ih, hwnd, NULL, self->hInstance, NULL);
            self->osButton = CreateWindowW(L"BUTTON", L"SYSTEME", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT,
                                           bx, iy + spacing*4, bw, bh, hwnd, (HMENU)5, self->hInstance, NULL);

            // Output box on the right
            self->outputBox = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_BORDER,
                                           260, 40, 520, 410, hwnd, NULL, self->hInstance, NULL);

            // Fonts for buttons
            SendMessageW(self->cpuButton, WM_SETFONT, (WPARAM)CreateFontW(12, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI"), TRUE);
            SendMessageW(self->ramButton, WM_SETFONT, (WPARAM)CreateFontW(12, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI"), TRUE);
            SendMessageW(self->diskButton, WM_SETFONT, (WPARAM)CreateFontW(12, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI"), TRUE);
            SendMessageW(self->gpuButton, WM_SETFONT, (WPARAM)CreateFontW(12, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI"), TRUE);
            SendMessageW(self->osButton,  WM_SETFONT, (WPARAM)CreateFontW(12, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI"), TRUE);

            // Load PNG icons via GDI+ and set as HBITMAP on the icon statics
            auto LoadPNG = [](const wchar_t* path)->HBITMAP {
                Bitmap* bmp = Bitmap::FromFile(path);
                if (!bmp) return NULL;
                HBITMAP hBitmap = NULL;
                Status st = bmp->GetHBITMAP(Color(0,0,0,0), &hBitmap);
                delete bmp;
                return st == Ok ? hBitmap : NULL;
            };
            self->cpuBmp = LoadPNG(L"Resources\\Icons\\cpu.png");
            self->ramBmp = LoadPNG(L"Resources\\Icons\\ram.png");
            self->diskBmp = LoadPNG(L"Resources\\Icons\\disk.png");
            self->gpuBmp = LoadPNG(L"Resources\\Icons\\gpu.png");
            self->osBmp  = LoadPNG(L"Resources\\Icons\\os.png");
            if (self->cpuBmp) SendMessageW(self->cpuIcon, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)self->cpuBmp);
            if (self->ramBmp) SendMessageW(self->ramIcon, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)self->ramBmp);
            if (self->diskBmp) SendMessageW(self->diskIcon, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)self->diskBmp);
            if (self->gpuBmp) SendMessageW(self->gpuIcon, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)self->gpuBmp);
            if (self->osBmp ) SendMessageW(self->osIcon,  STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)self->osBmp);
        }
        break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case 1: SetWindowTextW(self->outputBox, HardwareInfo::GetCPUInfo().c_str()); break;
                case 2: SetWindowTextW(self->outputBox, HardwareInfo::GetRAMInfo().c_str()); break;
                case 3: SetWindowTextW(self->outputBox, HardwareInfo::GetDiskInfo().c_str()); break;
                case 4: SetWindowTextW(self->outputBox, HardwareInfo::GetGPUInfo().c_str()); break;
                case 5: SetWindowTextW(self->outputBox, HardwareInfo::GetOSInfo().c_str()); break;
            }
            break;
        
        case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, COLOR_BG_SECONDARY);
            SetTextColor(hdc, COLOR_TEXT_SECONDARY);
            static HBRUSH editBrush = CreateSolidBrush(COLOR_BG_SECONDARY);
            return (INT_PTR)editBrush;
        }
        
        case WM_CTLCOLORBTN:
        {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, COLOR_BTN_ACTIVE);
            SetTextColor(hdc, COLOR_TEXT_SECONDARY);
            return (INT_PTR)self->btnBrush;
        }
        
        case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            HWND hCtl = (HWND)lParam;
            SetTextColor(hdc, COLOR_TEXT_SECONDARY);
            // Icon statics get icon background
            if (hCtl == self->cpuIcon || hCtl == self->ramIcon || hCtl == self->diskIcon || hCtl == self->gpuIcon || hCtl == self->osIcon)
            {
                SetBkColor(hdc, COLOR_ICON_BG);
                return (INT_PTR)self->iconBgBrush;
            }
            // Header uses header brush
            if (hCtl == self->infoHeader)
            {
                SetBkColor(hdc, COLOR_BG_MAIN);
                return (INT_PTR)self->headerBrush;
            }
            // Default static
            SetBkColor(hdc, COLOR_BG_SECONDARY);
            static HBRUSH defaultStatic = NULL;
            if (!defaultStatic) defaultStatic = CreateSolidBrush(COLOR_BG_SECONDARY);
            return (INT_PTR)defaultStatic;
        }

        case WM_DESTROY:
            // cleanup GDI+ bitmaps and brushes
            if (self)
            {
                if (self->cpuBmp) DeleteObject(self->cpuBmp);
                if (self->ramBmp) DeleteObject(self->ramBmp);
                if (self->diskBmp) DeleteObject(self->diskBmp);
                if (self->gpuBmp) DeleteObject(self->gpuBmp);
                if (self->osBmp)  DeleteObject(self->osBmp);
                if (self->btnBrush) DeleteObject(self->btnBrush);
                if (self->iconBgBrush) DeleteObject(self->iconBgBrush);
                if (self->headerBrush) DeleteObject(self->headerBrush);
            }
            PostQuitMessage(0);
            if (gdiPlusToken)
            {
                GdiplusShutdown(gdiPlusToken);
                gdiPlusToken = 0;
            }
            break;

        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}
