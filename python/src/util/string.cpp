//
// Created by Fidel on 2023/11/12.
//

#include "string.h"

#include <Windows.h>

std::string wstring_to_string(const std::wstring &str) {
    int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, NULL,
                                  NULL);
    char *buf = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, buf, len, NULL, NULL);
    std::string ret(buf);
    delete[] buf;
    return ret;
}