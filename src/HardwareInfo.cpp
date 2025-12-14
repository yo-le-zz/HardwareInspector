#include "HardwareInfo.h"
#include <windows.h>
#include <sstream>

namespace HardwareInfo
{
    std::wstring GetCPUInfo()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        std::wostringstream ss;
        ss << L"CPU: ";
        ss << L"cores=" << si.dwNumberOfProcessors << L"; ";
        ss << L"arch=" << (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? L"x64" : si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL ? L"x86" : L"Autre") << L"; ";
        ss << L"level=" << si.wProcessorLevel << L"; ";
        ss << L"rev=" << si.wProcessorRevision << L"; ";
        ss << L"pagesz=" << (si.dwPageSize / 1024) << L"KB\n";
        return ss.str();
    }

    std::wstring GetRAMInfo()
    {
        MEMORYSTATUSEX mem = {};
        mem.dwLength = sizeof(mem);
        GlobalMemoryStatusEx(&mem);
        std::wostringstream ss;
        ss << L"RAM: ";
        ss << L"total=" << (mem.ullTotalPhys / (1024 * 1024 * 1024)) << L"GB; ";
        ss << L"used=" << ((mem.ullTotalPhys - mem.ullAvailPhys) / (1024 * 1024)) << L"MB; ";
        ss << L"free=" << (mem.ullAvailPhys / (1024 * 1024)) << L"MB; ";
        ss << L"load=" << mem.dwMemoryLoad << L"%\n";
        ss << L"VM: total=" << (mem.ullTotalVirtual / (1024 * 1024 * 1024)) << L"GB; avail=" << (mem.ullAvailVirtual / (1024 * 1024 * 1024)) << L"GB\n";
        return ss.str();
    }

    std::wstring GetDiskInfo()
    {
        ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
        std::wostringstream ss;
        wchar_t drives[256];
        GetLogicalDriveStringsW(sizeof(drives)/sizeof(wchar_t), drives);
        int driveCount = 0;
        for (wchar_t* drive = drives; *drive; drive += wcslen(drive) + 1)
        {
            if (GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes))
            {
                unsigned long long totalGB = totalNumberOfBytes.QuadPart / (1024 * 1024 * 1024);
                unsigned long long freeGB = totalNumberOfFreeBytes.QuadPart / (1024 * 1024 * 1024);
                unsigned long long usedGB = totalGB - freeGB;
                int usage = totalGB ? (int)((usedGB * 100) / totalGB) : 0;
                ss << drive << L" total=" << totalGB << L"GB; free=" << freeGB << L"GB; used=" << usedGB << L"GB (" << usage << L"%)\n";
                driveCount++;
            }
        }
        if (driveCount == 0) ss << L"disks=0\n";
        return ss.str();
    }

    std::wstring GetGPUInfo()
    {
        std::wostringstream ss;
        ss << L"GPU: detected=0; APIs=DirectX,OpenGL,Vulkan; details=use WMI/registry\n";
        return ss.str();
    }

    std::wstring GetOSInfo()
    {
        OSVERSIONINFOEX osvi = {};
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((OSVERSIONINFO*)&osvi);
        std::wostringstream ss;
        ss << L"OS: version=" << osvi.dwMajorVersion << L"." << osvi.dwMinorVersion << L"; build=" << osvi.dwBuildNumber << L"; ";
        ss << L"platform=" << (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT ? L"WindowsNT" : L"Other") << L"; ";
        ss << L"sp=" << osvi.wServicePackMajor << L"." << osvi.wServicePackMinor << L"; ";
        DWORD size = 0; GetComputerNameW(NULL, &size);
        if (size == 0) size = 64;
        wchar_t* computerName = new wchar_t[size];
        GetComputerNameW(computerName, &size);
        ss << L"host=" << computerName << L"\n";
        delete[] computerName;
        return ss.str();
    }
}
