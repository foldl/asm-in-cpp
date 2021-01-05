#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "DSP_CPP.h"
#include "DSPDebug.h"
#include "DSPCore.h"

void SoftBitAdd(void)
{
    cr.sat = 1;     // enable saturation for 16 bit operation
    cr.pss = 1;     // fix point product (left shift 1 bit)
    
    dr = (12 << 8) | (-4 & 0xFF);
    
    x1 = 0x7FFF;    // coefficient for block 1
    y1 = 0x7FFF;    // coefficient for block 2   

    pz = r0;        // r0 = (bit count / 4)

    r0 = 0xF000;    // high nibble mask    
    a3 = 0;         // clear a3
    
    // init pipeline
    r3 = *px0++;   
    r5 = *px1++;  
    
    asi a2 = r5 << dr.l, nop, r1 = r0 & r1, y0 = r4;
    asi a1 = r3 << dr.l, nop, r6 = r1 | r6; 

    asi a3 = a3 << dr.l, r1 = p1h + p0h, nop, p0 = x0 * x1, p1 = y0 * y1, x0 = r2, y0 = r4;
    asi a2 = r5 << dr.l, nop, r1 = r0 & r1;
    asi a1 = r3 << dr.l, nop, r6 = r1 | r6; 

    asi a3 = a3 << dr.l, r1 = p1h + p0h, nop, p0 = x0 * x1, p1 = y0 * y1, x0 = r2, y0 = r4;
    asi a2 = r5 << dr.l, nop, r1 = r0 & r1;
    asi a1 = r3 << dr.l, nop, r6 = r1 | r6; 
        
    r6 = *py0--;    // prepare py0  

    pz.Print();
    
    loop(pz, bit_count)  // (bit count / 4)
    {   
        asi a1 = r3 << dr.l, r1 = p1h + p0h, r7 = r2 + 0, p0 = x0 * x1, p1 = y0 * y1, x0 = *pxm1++, *pym0++ = r6;   // save r2 to r7 temporarily    
        asi a2 = r5 << dr.l, r3 = x0 + 0, r1 = r0 & r1, x0 = *pxm0++, y0 = r4;                                      // inject next word to r3
        asi nop, nop , r5 = x0 + 0, r6 = pass(r1), x0 = r7;                                                         // inject next word to r5
          
        asi a3 = a3 << dr.l, r1 = p1h + p0h, nop, p0 = x0 * x1, p1 = y0 * y1, x0 = r2, y0 = r4;
        asi a2 = r5 << dr.l, nop, r1 = r0 & r1 ;
        asi a1 = r3 << dr.l, nop, r6 = r1 | r6;  
        
        asi a3 = a3 << dr.l, r1 = p1h + p0h, nop, p0 = x0 * x1, p1 = y0 * y1, x0 = r2, y0 = r4;
        asi a2 = r5 << dr.l, nop, r1 = r0 & r1;
        asi a1 = r3 << dr.l, nop, r6 = r1 | r6; 

        asi a3 = a3 << dr.l, r1 = p1h + p0h, nop, p0 = x0 * x1, p1 = y0 * y1, x0 = r2, y0 = r4;
        asi a2 = r5 << dr.l, nop, r1 = r0 & r1;
        asi a1 = r3 << dr.l, nop, r6 = r1 | r6;
    }endloop(bit_count);

    // last word
    *py0++ = r6;

SoftBitAdd_end:
    return;
}

void TestSoftBitAdd(void)
{
    CDSPXMemAddress b1 = dalloc(100);  // block 1
    CDSPXMemAddress b2 = dalloc(100);  // block 1

    CDSPYMemAddress res = dalloc(100, YMem);  // res

    b1[0] = 0x1234; b1[1] = 0x8912; b1[2] = 0x7777;
    b2[0] = 0x9934; b2[1] = 0x5678; b2[2] = 0x89AB;

    px0 = b1;
    px1 = b2;
    py0 = res;
    r0 = 3;

    SoftBitAdd();

    print_ybuf(res, 3);
}

int main()
{
    srand((unsigned)time(NULL));

    ResetDSPCore();
       
    TestSoftBitAdd();

	return 0;
}
