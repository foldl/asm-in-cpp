#include <stdlib.h> 
#include <ctype.h>
#include <stdio.h>

#include "DSPIO.h"

#ifdef _COLORFUL_DIS
#include <windows.h>

static HANDLE hConsole = INVALID_HANDLE_VALUE;
static WORD   OldColor = 0;

#define GetConsoleHandle()                          \
    if (INVALID_HANDLE_VALUE == hConsole)           \
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
    if (INVALID_HANDLE_VALUE == hConsole)           \
        return

// #define _RAISE_EXCEPTION

void SetColor(WORD color)
{
    GetConsoleHandle();
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(hConsole, &info))
        OldColor = info.wAttributes;
    WORD CommonMask =   COMMON_LVB_LEADING_BYTE 
                      | COMMON_LVB_TRAILING_BYTE
                      | COMMON_LVB_GRID_HORIZONTAL 
                      | COMMON_LVB_GRID_LVERTICAL 
                      | COMMON_LVB_GRID_RVERTICAL 
                      | COMMON_LVB_REVERSE_VIDEO 
                      | COMMON_LVB_UNDERSCORE;
    WORD ColorMask =    FOREGROUND_BLUE 
                      | FOREGROUND_GREEN 
                      | FOREGROUND_RED 
                      | FOREGROUND_INTENSITY 
                      | BACKGROUND_BLUE 
                      | BACKGROUND_GREEN 
                      | BACKGROUND_RED 
                      | BACKGROUND_INTENSITY;

    SetConsoleTextAttribute(hConsole, 
                            static_cast<WORD>((OldColor & CommonMask) | (color & ColorMask)));
}

void RestoreColor(void)
{
    GetConsoleHandle();
    SetConsoleTextAttribute(hConsole, OldColor);
}

bool IsStrHead(const char *str, const char *head)
{
    if ((NULL == str) || (NULL == head))
        return false;
    if (strlen(str) < strlen(head))
        return false;
    const char *p1 = str;
    const char *p2 = head;
    while (*p2 != '\0')
    {
        if (tolower(*p1++) != tolower(*p2++))
            return false;
    }
    return true;
}

void dspprint(const char *format, ...)
{
    GetConsoleHandle();
    char str[1024];
    str[0] = '\0';
    va_list    args;
    va_start(args, format);
    
    vsprintf(str, format, args);
    const int len = strlen(str);
    if (len < 1)
        return;

    WORD color =  FOREGROUND_BLUE 
                | FOREGROUND_GREEN 
                | FOREGROUND_RED;
    
    if (IsStrHead(str, "WARNING"))
        color =   FOREGROUND_GREEN | FOREGROUND_RED;    
    else if (IsStrHead(str, "ERROR") || IsStrHead(str, "INTERNAL ERROR"))
        color =   FOREGROUND_RED;
    else if (IsStrHead(str, "EXCEPTION"))
        color =   FOREGROUND_RED;
    else if (   IsStrHead(str, "PROG FATAL") || IsStrHead(str, "INTERNAL FATAL")
             || IsStrHead(str, "FATAL") || IsStrHead(str, "!"))
    {
#ifdef _RAISE_EXCEPTION
        throw exception(str);      
#endif
        color =   FOREGROUND_RED | FOREGROUND_INTENSITY;
    }
    else if (IsStrHead(str, "INFO"))
        color =   FOREGROUND_GREEN;
    else if (IsStrHead(str, "|---") || IsStrHead(str, "|___"))
        color =   FOREGROUND_BLUE 
                | FOREGROUND_GREEN 
                | FOREGROUND_RED
                | BACKGROUND_BLUE;
    else;

    bool bCR = false;
    if ('\n' == str[len - 1])
    {
        bCR = true;
        str[len - 1] = '\0';
    }

    SetColor(color);
    printf(str);
    RestoreColor();

    if (bCR)
        printf("\n");
}
#endif
