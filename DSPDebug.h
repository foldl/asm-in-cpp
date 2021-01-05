/*********************************************************************
*                   DSP Debug marcoes
*********************************************************************/
#ifndef _DSP_DEBUG_H
#define _DSP_DEBUG_H

#include "DSPIO.h"

#define print(var)  dspprint(#var " = %d\n", var)

#define print_xbuf(buf, len) print_buffer (buf, len)
#define print_ybuf(buf, len) print_buffery(buf, len)

#define print_buffer(buf, len)                              \
{                                                           \
    dspprint("|--------- %9s ----------|\n", #buf);          \
    CDSPXMemAddress x = buf;                                \
    for (int i = 0; i < len; i++)                           \
    {                                                       \
        dspprint("\t"); (*x++).Print();                     \
    }                                                       \
    dspprint("|__________________________________|\n");     \
    printf("\n");                                           \
}                                               

#define print_buffery(buf, len)                             \
{                                                           \
    dspprint("|--------- " #buf " ----------|\n");          \
    CDSPYMemAddress y = buf;                                \
    for (int i = 0; i < len; i++)                           \
    {                                                       \
        dspprint("\t"); (*y++).Print();                     \
    }                                                       \
    dspprint("|__________________________________|\n");     \
    printf("\n");                                           \
}                                               

// modify the definition of call
#ifdef dcall
    #undef dcall
#endif
#define dcall(fun) call_with_sp_check(fun) 

#define call_with_sp_check(fun)                                             \
{                                                                           \
    const int oldsp = static_cast<int>(sp);                                 \
    fun();                                                                  \
    const int newsp = static_cast<int>(sp);                                 \
    if (oldsp != newsp)                                                     \
    {                                                                       \
        dspprint("PROG FATAL: stack leak occured in '" #fun                 \
                 "': 0x%04X <--> 0x%04X\n", oldsp, newsp);                  \
        sp = oldsp;                                                         \
    }                                                                       \
}

#endif