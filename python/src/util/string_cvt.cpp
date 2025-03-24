#include "string_cvt.h"

#include <windows.h>

std::string wstring_to_string(const std::wstring &str) {
    const int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0,
                                        nullptr, nullptr);
    const auto buf = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, buf, len, nullptr,
                        nullptr);
    std::string ret(buf);
    delete[] buf;
    return ret;
}