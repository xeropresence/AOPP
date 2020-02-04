#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


class FlashManager
{
public:
    static FlashManager& getInstance()
    {
        static FlashManager    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return instance;
    }


    void Flash() const;


private:
    FlashManager(); // Constructor? (the {} brackets) are needed here.

    HWND hwnd = nullptr;
public:
    FlashManager(FlashManager const&) = delete;
    void operator=(FlashManager const&) = delete;

};
