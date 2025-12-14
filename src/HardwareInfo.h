#pragma once
#include <string>

namespace HardwareInfo
{
    std::wstring GetCPUInfo();
    std::wstring GetRAMInfo();
    std::wstring GetDiskInfo();
    std::wstring GetGPUInfo();
    std::wstring GetOSInfo();
}
