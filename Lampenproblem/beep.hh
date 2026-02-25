#pragma once

#ifndef CROSS_PLATFORM_BEEP_H
#define CROSS_PLATFORM_BEEP_H

#include <cstdint>
#include <cstdlib>

#if defined(_WIN32)
    #include <windows.h>

#elif defined(__linux__)
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <linux/kd.h>

#endif

inline bool beep(uint32_t freq, uint32_t dauer)
{
#if defined(_WIN32)

    return Beep(freq, dauer) != 0;

#elif defined(__linux__)

    if (freq == 0)
        return false;

    // Versuch 1: Direktzugang
    int fd = open("/dev/console", O_WRONLY);
    if (fd >= 0)
    {
        uint32_t ton = (dauer << 16) | (1193180 / freq);

        if (ioctl(fd, KDMKTONE, ton) == 0)
        {
            close(fd);
            return true;
        }

        close(fd);
    }

    // Versuch 2: beep kommando
    {
        // Typische Syntax des beep tools
        // -f freq (Hz)
        // -l dauer (ms)
        char cmd[128];
        std::snprintf(cmd, sizeof(cmd), "beep -f %u -l %u 2>/dev/null", freq, dauer);

        int ergebnis = std::system(cmd);

        if (ergebnis == 0)
            return true;
    }

    // Versuch 3: Terminal Bell
    {
        std::printf("\a");
        std::fflush(stdout);
        return false;  // diese abgeranzte scheiße zählt als fehler (weil keine tonhöhe)
    }

#else

    return false;

#endif
}

#endif // CROSS_PLATFORM_BEEP_H
