#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "X86Flavor.h"

int main()
{
    srand((unsigned)time(NULL));

    ResetDSPCore();
       
    r0 = 100;
    MOV R1, R2;
    ADD R0, R1;

    r0.Print();

	return 0;
}