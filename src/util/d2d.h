#pragma once

#include <windows.h>

#include <string>

std::string ansi_to_utf8(const std::string& ansi_str) {
    int wlen = MultiByteToWideChar(CP_ACP, 0, ansi_str.c_str(), -1, nullptr, 0);
    std::wstring wstr(wlen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansi_str.c_str(), -1, &wstr[0], wlen);

    int utf8len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8str(utf8len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8str[0], utf8len, nullptr, nullptr);

    return utf8str;
}
