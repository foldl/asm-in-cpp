/*********************************************************************
*                   DSP data type definitions
*********************************************************************/
#ifndef _DSP_TYPES_H
#define _DSP_TYPES_H

#include <stdint.h>

typedef uint32_t    DWORD;
typedef uint16_t    WORD;
typedef int8_t      _fix_8;
typedef int16_t     _fix_16;
typedef int32_t     _fix_32;
typedef int64_t     _fix_40;
typedef uint16_t    _ufix_16;

#define STRUCT(name)    \
{                       \
    int i = 0;

#define STRUCT_ELE(name, len)  name = i; i += len;

#define ENDSTRUCT(name)                     \
    ; /* ## cannot at the beginning */      \
    ##name##_SIZE   = i;                    \
}

#endif
