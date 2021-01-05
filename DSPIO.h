#ifndef _DSP_IO_H
#define _DSP_IO_H

#include <stdio.h>

#define _COLORFUL_DIS

#ifdef _COLORFUL_DIS
    void dspprint(const char *format, ...);
#else
    #define dspprint printf
#endif

#endif