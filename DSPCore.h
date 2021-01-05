  
#ifndef _DSP_CORE_H
#define _DSP_CORE_H

#include "DSP_CPP.h"

extern CDSPControlRegister cr;
extern CDSPStatusRegister  sr;

extern CDSPBitObject & lof;
extern CDSPBitObject & z;
extern CDSPBitObject & of;
extern CDSPBitObject & c;
extern CDSPBitObject & n;
extern CDSPBitObject & odddd;
extern CDSPBitObject & ac;
extern CDSPBitObject & lnz;

extern CDSPStackedObject<CDSP16BitRegister>    lean;
extern CDSPStackedObject<CDSP16BitRegister>    lcn;
extern CDSPStackedObject<CDSP16BitRegister>    lsa;

// common registers
extern CDSP16BitDCURegister r0;
extern CDSP16BitDCURegister r1;
extern CDSP16BitDCURegister r2;
extern CDSP16BitDCURegister r3;
extern CDSP16BitDCURegister r4;
extern CDSP16BitDCURegister r5;
extern CDSP16BitDCURegister r6;
extern CDSP16BitDCURegister r7;

extern CDSP16BitRegister o0;
extern CDSP16BitRegister o1;
extern CDSP16BitRegister o2;
extern CDSP16BitRegister o3;

extern CDSPCombinedObject a0;
extern CDSPCombinedObject a1;
extern CDSPCombinedObject a2;
extern CDSPCombinedObject a3;

extern CDSPCombinedObject & o0r1r0;  
extern CDSPCombinedObject & o1r3r2;  
extern CDSPCombinedObject & o2r5r4;  
extern CDSPCombinedObject & o3r7r6;                      
extern CDSPCombinedObject &   r1r0;  
extern CDSPCombinedObject &   r3r2;  
extern CDSPCombinedObject &   r5r4;  
extern CDSPCombinedObject &   r7r6;

extern CDSPCombinedObject y0x0;
extern CDSPCombinedObject y1x1;

extern CDSPCombinedObject o1o0;
extern CDSPCombinedObject o3o2;

extern CDSP16BitRegister x0;
extern CDSP16BitRegister y0;
extern CDSP16BitRegister x1;
extern CDSP16BitRegister y1;

extern CDSP16BitRegister        p0l;
extern CDSPProductHighObject    p0h;
extern CDSP8BitRegister         p0o;

extern CDSP16BitRegister        p1l;
extern CDSPProductHighObject    p1h;
extern CDSP8BitRegister         p1o;

extern CDSPCombinedObject p0;
extern CDSPCombinedObject p1;

// addressing
extern CDSPPointerRegister pxm0;
extern CDSPPointerRegister pxs0;
extern CDSPPointerRegister pxa0;
extern CDSPPointerRegister pxm1;
extern CDSPPointerRegister pxs1;
extern CDSPPointerRegister pxa1;
extern CDSPPointerRegister pym0;
extern CDSPPointerRegister pys0;
extern CDSPPointerRegister pya0;
extern CDSPPointerRegister pym1;
extern CDSPPointerRegister pys1;
extern CDSPPointerRegister pya1;

extern CDSP16BitRegister sxm0;
extern CDSP16BitRegister sxa0;
extern CDSP16BitRegister sxm1;
extern CDSP16BitRegister sxa1;

extern CDSP16BitRegister sym0;
extern CDSP16BitRegister sya0;
extern CDSP16BitRegister sym1;
extern CDSP16BitRegister sya1;

extern CDSP16BitRegister saxm;
extern CDSP16BitRegister saxa;
extern CDSP16BitRegister eaxm;
extern CDSP16BitRegister eaxa;

extern CDSP16BitRegister saym;
extern CDSP16BitRegister saya;
extern CDSP16BitRegister eaym;
extern CDSP16BitRegister eaya;

extern CDSP16BitRegister saxs0;
extern CDSP16BitRegister saxs1;
extern CDSP16BitRegister eaxs0;
extern CDSP16BitRegister eaxs1;

extern CDSP16BitRegister says0;
extern CDSP16BitRegister says1;
extern CDSP16BitRegister eays0;
extern CDSP16BitRegister eays1;

// magic
extern CDSPMagicPointerRegister px0;
extern CDSPMagicPointerRegister px1;
extern CDSPMagicPointerRegister py0;
extern CDSPMagicPointerRegister py1;

extern CDSPMagicObject sx0;
extern CDSPMagicObject sx1;
extern CDSPMagicObject sy0;
extern CDSPMagicObject sy1;

extern CDSPMagicObject sax;
extern CDSPMagicObject eax;
extern CDSPMagicObject say;
extern CDSPMagicObject eay;

// AXU
extern CDSPPZRegister    pz;
extern CDSPPZRegister    pzp;
extern CDSP16BitRegister bm;
extern CDSP16BitRegister bmp;

// misc
extern CDSPRecordedPointerRegister  sp;
extern CDSPDrRegister               dr;
extern CDSP16BitRegister            mcr;
extern CDSP16BitRegister            ra;

extern CDSPProductRoundFlag q;

extern CDSPObjectPool      gREALObjectPool;
extern CDSPMemoryPool      gREALMemoryPool;
#endif