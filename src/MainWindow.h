#pragma once
#include <windows.h>

class MainWindow
{
public:
    MainWindow(HINSTANCE hInst);
    void Show(int nCmdShow);

private:
    HINSTANCE hInstance;
    HWND hwnd;
    HWND cpuButton, ramButton, diskButton, gpuButton, osButton, outputBox, infoHeader;
    HBITMAP cpuBmp, ramBmp, diskBmp, gpuBmp, osBmp;
    HWND cpuIcon, ramIcon, diskIcon, gpuIcon, osIcon;
    HBRUSH btnBrush, iconBgBrush, headerBrush;
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
