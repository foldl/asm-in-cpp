      
// DSP_CPP.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DSP_CPP.h"

bool  bParallel = false;
DWORD ParallelSignature = 0;
bool getparallel(void)
{
    return bParallel;
}

DWORD getsig(void)
{
    return ParallelSignature;
}

void enterparallel(void)
{
    bParallel = true;
    ++ParallelSignature;
}

void leaveparallel(void)
{
    bParallel = false;
}

bool bProductRoundPadding = false;
CDSPProductHighObject *pProductHigh = NULL;

bool getprpadding(void)
{
    return bProductRoundPadding;
}

void setprpadding(const bool b)
{
    bProductRoundPadding = b;
}

void setph(CDSPProductHighObject *p)
{
    pProductHigh = p;
}

CDSPProductHighObject *getph(void)
{
    return pProductHigh;
}

_fix_16 getsax(void);
_fix_16 geteax(void);
_fix_16 getsay(void);
_fix_16 geteay(void);

CDSPControlRegister cr;
CDSPStatusRegister  sr;

CDSPBitObject & lof  = sr.lof;
CDSPBitObject & z    = sr.z;
CDSPBitObject & of   = sr.of;
CDSPBitObject & c    = sr.c;
CDSPBitObject & n    = sr.n;
CDSPBitObject & odd  = sr.odd;
CDSPBitObject & ac   = sr.ac;
CDSPBitObject & lnz  = sr.lnz;

CDSPLoopManager gLoopManager;

void before_lsa_pop(void);

CDSPStackedObject<CDSP16BitRegister>    lean("lean", true,  true,  false);
CDSPStackedObject<CDSP16BitRegister>    lcn ("lcn",  true,  true,  false);
CDSPStackedObject<CDSP16BitRegister>    lsa ("lsa",  false, false, true, before_lsa_pop);

int __fastcall set_lsa(const DWORD address)
{  
//  printf("%08X\n", address);
    int res = gLoopManager.AddAddress(address);
    lsa = static_cast<_fix_16>(res);
    return res;
}

DWORD __fastcall get_lsa(void)
{
    return gLoopManager.GetAddress(static_cast<int>(lsa.GetValue()));
}

void before_lsa_pop(void)
{
    gLoopManager.RemoveAddress(static_cast<int>(lsa.GetValue()));
}

char * PrintInt(char *str, const _fix_40 value, const int BitLen)
{
    if (NULL == str)
        return NULL;
    str[0] = '\0';
    switch (BitLen) {
    case 1:
        if (value == 0)
            strcpy(str, "0");
        else
            strcpy(str, "1");
    	break;
    case 2:
        sprintf(str, "%d", value & 0x0003);
    	break;
    case 4:
        sprintf(str, "%01X", value & 0x000F);
    	break;
    case 8:
        sprintf(str, "%02X", value & 0x00FF);
    	break;
    case 16:
        sprintf(str, "%04X", value & 0xFFFF);
    	break;
    case 32:
        sprintf(str, "%08X", value & 0xFFFFFFFF);
    	break;
    default:
        printf("INTERNAL FATAL: PrintInt() BitLen = %d\n", BitLen);
    }
    return str;
}

CDSPObjectPool gREALObjectPool;

int RegisterObject(CDSPObject *pObj)
{
    return gREALObjectPool.AddRegister(pObj);
}

const CDSPPointerRegister postinc(CDSPPointerRegister &reg, const CDSPObject &offset)
{
    return postinc(reg, offset.GetValue());
}

const CDSPPointerRegister postdec(CDSPPointerRegister &reg, const CDSPObject &offset)
{
    return postdec(reg, offset.GetValue());
}

const CDSPPointerRegister postinc(CDSPPointerRegister &reg, const _fix_40     offset)
{
    CDSPPointerRegister result(reg);
    reg.SetValue(reg.GetValue() + offset);
    return result;
}

const CDSPPointerRegister postdec(CDSPPointerRegister &reg, const _fix_40     offset)
{
    CDSPPointerRegister result(reg);
    reg.SetValue(reg.GetValue() - offset);
    return result;
}

CDSPMemoryPool gREALMemoryPool;

CDSPMemoryObject & GetMemoryObject(const int Address, const CMemClass MemClass)
{
    return gREALMemoryPool.GetMemoryObject(Address, MemClass);
}

CDSPMemoryObject & xmem(const int Address)
{
    return GetMemoryObject(Address, XMem);
}

_fix_16 div(CDSP16BitRegister & r0, CDSP16BitRegister & x0, CDSP16BitRegister & dr)
{
    _fix_16 r = static_cast<_fix_16>(r0.GetValue());
    _fix_16 x = static_cast<_fix_16>(x0.GetValue());
    _fix_16 d = static_cast<_fix_16>(dr.GetValue());
    _fix_16 res = fixdiv(r, x, d);
    r0.SetValue(r);
    x0.SetValue(x);
    dr.SetValue(d);
    return res;
}

_fix_16 norm(const CDSPObject & a0)
{
    return fixnorm(a0.GetValue());
}

_fix_16 norm(const _fix_40 value)
{
    return fixnorm(value);
}

_fix_16 pass(const CDSP16BitRegister & r0)
{
    return fixpass<_fix_16>(static_cast<_fix_16>(r0.GetValue()));
}

_fix_16 pass(const int value)
{
    return fixpass<_fix_16>(static_cast<_fix_16>(value));
}

_fix_16 bitcount(const CDSP16BitRegister &reg)
{
    _fix_16 res  = 0;
    WORD    flag = 1;
    _fix_16 v    = static_cast<_fix_16>(reg.GetValue());
    for (int i = 0; i < 16; i++)
    {
        res = static_cast<_fix_16>(res + ((v & flag) != 0 ? 1 : 0));
        flag <<= 1;
    }
    return res;
}

_fix_16 max(const CDSP16BitRegister & a, const CDSP16BitRegister & b)
{
    return max(static_cast<_fix_16>(a.GetValue()), static_cast<_fix_16>(b.GetValue()));
}

_fix_16 max(const _fix_16 a, const _fix_16 b)
{
    _fix_16 res = (a > b) ? a : b;
    setstatus<_fix_16>(res, res);
    setc(a >= b);
    return res;
}

_fix_40 lmax(const CDSPObject & a, const CDSPObject & b)
{
    return lmax(a.GetValue(), b.GetValue());
}

_fix_40 lmax(const _fix_40 a, const _fix_40 b)
{
    _fix_40 res = (a > b) ? a : b;
    setstatus<_fix_40>(res, res);
    setc(a >= b);
    setac((a & 0xFFFFFFFFFFUL) >= (b & 0xFFFFFFFFFFUL));
    return res;
}

_fix_16 fixdiv(_fix_16 &r0, _fix_16 &x0, _fix_16 &dr)
{
    int c_local = 0;
    if (getca() == ((x0 & 0x8000) >> 15))
    {
        c_local = ((r0 - x0) & 0x8000) >> 15;
        setc(c_local != 0);
        dr = static_cast<_fix_16>((dr << 1) | (c_local != 0 ? 0 : 1));
        r0 = static_cast<_fix_16>((r0 - x0) << 1);
    }
    else
    {
        c_local = ((r0 + x0) & 0x8000) >> 15;
        setc(c_local != 0);
        dr = static_cast<_fix_16>((dr << 1) | (c_local != 0 ? 0 : 1));
        r0 = static_cast<_fix_16>((r0 + x0) << 1);
    }
    return r0;
};

// round 40bit t0 24bit
_fix_40 fixround(_fix_40 a)
{
    _fix_40 res = ((a & 0xFFFFFF0000) >> 1);
    if ((a & 0x8000) != 0)
        res++;
    else;
    return setstatus<_fix_40>(res, res);
};

_fix_40 fixsat(_fix_40 a)
{
    _fix_40 res = a;
    if (a > 0x7FFFFFFF)    
        res = 0x7FFFFFFF;
    else if (a < -_fix_40(0x80000000))
        res = 0x80000000;
    else;
    setz(res == 0);
    setn(res < 0);
 
    setof((a > 0x7FFFFFFF) || (a < -_fix_40(0x80000000)));
    setac(false);
    setc(false);

    return res;
};

_fix_16 fixnorm(_fix_40 a)
{
    _fix_40 temp = a & 0xFFFFFFFFFF;
    _fix_16 res = 0;
    if (temp != 0)
    {
        if (temp < 0)
            temp = ~temp;
        else;
        do {
            res++;
            temp <<= 1;
        } while ((temp & 0x8000000000) == 0);
        res -= 9;
    }
    else;
    
    return static_cast<_fix_16>(setstatus<_fix_16>(res, res));
};

_fix_16 getbit(const CDSPObject &bmp, const CDSPPZRegister &pzp)
{
    return CDSPPZRegister::GetBit(bmp, pzp);
}

_fix_16 getnibble(const CDSPObject &bmp, const CDSPPZRegister &pzp)
{
    return CDSPPZRegister::GetNibble(bmp, pzp);
}

_fix_16 getcmi(const CDSPObject &bmp, const CDSPPZRegister &pzp)
{
    return CDSPPZRegister::GetCMI(bmp, pzp);
}

_fix_16 getbyte(const CDSPObject &bmp, const CDSPPZRegister &pzp)
{
    return CDSPPZRegister::GetByte(bmp, pzp);
}

_fix_16 putbit(const CDSPObject &bmp, const CDSPPZRegister &pzp)
{
    return CDSPPZRegister::PutBit(bmp, pzp);
}

_fix_16 putn(const CDSPPZRegister &pzp)
{
    CDSPObject temp("temp");
    temp = getn();
    _fix_16 res = putbit(temp, pzp);
    return res;
}

_fix_16 putnibble(const CDSPObject &bmp, const CDSPPZRegister &pzp)
{
    return CDSPPZRegister::PutNibble(bmp, pzp);
}

_fix_16 expand(const CDSPObject &bm)
{
    const int v = static_cast<int>(bm.GetValue());
    int res  = (((getpage() & 0x8) >> 3) * 0xF) << 12;
        res |=   (getpage() & 0xF) << 8;
    if ((v & 0x8000) != 0)
        res |= (v & 0x7800) >> 7;
    else
        res |= (~v & 0x7800) >> 7;
    if ((v & 0x0080) != 0)
        res |= (v & 0x78) >> 3;
    else
        res |= (~v & 0x78) >> 3;
    return static_cast<_fix_16>(res);
}

_fix_16 reverse(const CDSPObject &bm)
{
    const DWORD v = static_cast<WORD>(bm.GetValue());
    DWORD res = 0;
    for (int i = 0; i < 16; i++)
    {
        res += (v & (1 << i)) << (31 - 2 * i);
    }
        
    return static_cast<_fix_16>((res & 0xFFFF0000) >> 16);
}

_fix_16 putbyte(const CDSPObject &bmp, const CDSPPZRegister &pzp)
{
    return CDSPPZRegister::PutByte(bmp, pzp);
}

int getam (void)
{
	return static_cast<int>(sr.am.GetValue());
}

int getgie(void)
{
	return static_cast<int>(sr.gie.GetValue());
}

int getlof(void)
{
	return static_cast<int>(sr.lof.GetValue());
}

int getz  (void)
{
	return static_cast<int>(sr.z.GetValue());
}

int getof (void)
{
	return static_cast<int>(sr.of.GetValue());
}

int getca (void)
{
	return static_cast<int>(sr.c.GetValue());
}

int getn  (void)
{
	return static_cast<int>(sr.n.GetValue());
}

int getodd(void)
{
	return static_cast<int>(sr.odd.GetValue());
}

int getac (void)
{
	return static_cast<int>(sr.ac.GetValue());
}

int getlnz(void)
{
	return static_cast<int>(sr.lnz.GetValue());
}

int setlof(const bool flag)
{
    if (flag)
        sr.lof = 1;
    else;
    return  static_cast<int>(sr.lof.GetValue());
}

int setz  (const bool flag)
{
    if (!flag)
        setlnz(true);
    else;
    return static_cast<int>((sr.z = flag ? 1 : 0).GetValue());
}

int setof (const bool flag)
{
    if (flag)
        setlof(true);
    else;
	return static_cast<int>((sr.of = flag ? 1 : 0).GetValue());
}

int setc  (const bool flag)
{
	return static_cast<int>((sr.c = flag ? 1 : 0).GetValue());
}

int setn  (const bool flag)
{
	return static_cast<int>((sr.n = flag ? 1 : 0).GetValue());
}

int setodd(const bool flag)
{
	return static_cast<int>((sr.odd = flag ? 1 : 0).GetValue());
}

int setac (const bool flag)
{
	return static_cast<int>((sr.ac = flag ? 1 : 0).GetValue());
}

int setlnz(const bool flag)
{
	return static_cast<int>((sr.lnz = flag ? 1 : 0).GetValue());
}

int getpss(void)
{
	return static_cast<int>(cr.pss.GetValue());
}

int getprm(void)
{
	return static_cast<int>(cr.prm.GetValue());
}

int getsat(void)
{
	return static_cast<int>(cr.sat.GetValue());
}

int getbsm(void)
{
	return static_cast<int>(cr.bsm.GetValue());
}

int getnie(void)
{
	return static_cast<int>(cr.nie.GetValue());
}

int getsa (void)
{
	return static_cast<int>(cr.sa.GetValue());
}

int getsb (void)
{
	return static_cast<int>(cr.sb.GetValue());
}

int getbr (void)
{
	return static_cast<int>(cr.br.GetValue());
}

// pre-allocate
int prealloc(const int Size, const CMemClass MemClass, const CMemoryAllocationScheme Scheme, const int Address)
{
    int res = dalloc(Size, MemClass, Scheme, Address);
    if (res >= 0)
        dfree(res);
    else;
    return res;
}

int dalloc(const int Size, const CMemClass MemClass, const CMemoryAllocationScheme Scheme, const int Address)
{
    return gREALMemoryPool.AllocateMemory(Scheme, Size, MemClass, Address);
}

int dfree(const int Address, const CMemClass MemClass, const int Size)
{
    return gREALMemoryPool.FreeMemory(Address, MemClass, Size);
}

int  dlen(const int Address, const CMemClass MemClass)
{
    return gREALMemoryPool.MemoryBlockLen(Address, MemClass);
}

// common registers
CDSP16BitDCURegister r0("r0");
CDSP16BitDCURegister r1("r1");
CDSP16BitDCURegister r2("r2");
CDSP16BitDCURegister r3("r3");
CDSP16BitDCURegister r4("r4");
CDSP16BitDCURegister r5("r5");
CDSP16BitDCURegister r6("r6");
CDSP16BitDCURegister r7("r7");

CDSP16BitRegister o0("o0");
CDSP16BitRegister o1("o1");
CDSP16BitRegister o2("o2");
CDSP16BitRegister o3("o3");

CDSPCombinedObject a0("a0");
CDSPCombinedObject a1("a1");
CDSPCombinedObject a2("a2");
CDSPCombinedObject a3("a3");

CDSPCombinedObject & o0r1r0 = a0;
CDSPCombinedObject & o1r3r2 = a1;
CDSPCombinedObject & o2r5r4 = a2;
CDSPCombinedObject & o3r7r6 = a3;
CDSPCombinedObject &   r1r0 = a0;  
CDSPCombinedObject &   r3r2 = a1;  
CDSPCombinedObject &   r5r4 = a2;  
CDSPCombinedObject &   r7r6 = a3;  

CDSPCombinedObject y0x0("y0x0");
CDSPCombinedObject y1x1("y1x1");

CDSPCombinedObject o1o0("o1o0");
CDSPCombinedObject o3o2("o3o2");

CDSP16BitRegister x0("x0");
CDSP16BitRegister y0("y0");
CDSP16BitRegister x1("x1");
CDSP16BitRegister y1("y1");

CDSP16BitRegister       p0l("p0l");
CDSPProductHighObject   p0h("p0h");
CDSP8BitRegister        p0o("p0o");

CDSP16BitRegister       p1l("p1l");
CDSPProductHighObject   p1h("p1h");
CDSP8BitRegister        p1o("p1o");

CDSPCombinedObject p0("p0");
CDSPCombinedObject p1("p1");

// addressing
CDSPPointerRegister pxm0("pxm0", XMem, getsax, geteax);
CDSPPointerRegister pxs0("pxs0", XMem, getsax, geteax);
CDSPPointerRegister pxa0("pxa0", XMem, getsax, geteax);
CDSPPointerRegister pxm1("pxm1", XMem, getsax, geteax);
CDSPPointerRegister pxs1("pxs1", XMem, getsax, geteax);
CDSPPointerRegister pxa1("pxa1", XMem, getsax, geteax);
CDSPPointerRegister pym0("pym0", YMem, getsay, geteay);
CDSPPointerRegister pys0("pys0", YMem, getsay, geteay);
CDSPPointerRegister pya0("pya0", YMem, getsay, geteay);
CDSPPointerRegister pym1("pym1", YMem, getsay, geteay);
CDSPPointerRegister pys1("pys1", YMem, getsay, geteay);
CDSPPointerRegister pya1("pya1", YMem, getsay, geteay);

CDSP16BitRegister sxm0("sxm0");
CDSP16BitRegister sxa0("sxa0");
CDSP16BitRegister sxm1("sxm1");
CDSP16BitRegister sxa1("sxa1");

CDSP16BitRegister sym0("sym0");
CDSP16BitRegister sya0("sya0");
CDSP16BitRegister sym1("sym1");
CDSP16BitRegister sya1("sya1");

CDSP16BitRegister saxm("saxm");
CDSP16BitRegister saxa("saxa");
CDSP16BitRegister eaxm("eaxm");
CDSP16BitRegister eaxa("eaxa");

CDSP16BitRegister saym("saym");
CDSP16BitRegister saya("saya");
CDSP16BitRegister eaym("eaym");
CDSP16BitRegister eaya("eaya");

CDSP16BitRegister saxs0("saxs0");
CDSP16BitRegister saxs1("saxs1");
CDSP16BitRegister eaxs0("eaxs0");
CDSP16BitRegister eaxs1("eaxs1");

CDSP16BitRegister says0("says0");
CDSP16BitRegister says1("says1");
CDSP16BitRegister eays0("eays0");
CDSP16BitRegister eays1("eays1");

// magic
CDSPMagicPointerRegister px0("px0", getam, XMem);
CDSPMagicPointerRegister px1("px1", getam, XMem);
CDSPMagicPointerRegister py0("py0", getam, YMem);
CDSPMagicPointerRegister py1("py1", getam, YMem);

CDSPMagicObject sx0("sx0", getam);
CDSPMagicObject sx1("sx1", getam);
CDSPMagicObject sy0("sy0", getam);
CDSPMagicObject sy1("sy1", getam);

CDSPMagicObject sax("sax", getam);
CDSPMagicObject eax("eax", getam);
CDSPMagicObject say("say", getam);
CDSPMagicObject eay("eay", getam);

// AXU
CDSPPZRegister    pz("pz");
CDSPPZRegister    pzp("pzp");
CDSP16BitRegister bm("bm");
CDSP16BitRegister bmp("bmp");

// misc
CDSPRecordedPointerRegister sp("sp",  XMem, NULL, NULL);

CDSPDrRegister      dr;

CDSP16BitRegister   mcr("mcr");

CDSP16BitRegister   ra("ra");

CDSPProductRoundFlag q;

int getpage(void)
{
    return static_cast<int>(mcr.GetValue() & 0xF);
}

_fix_16 getsax(void)
{
    return static_cast<_fix_16>(sax.GetValue());
}

_fix_16 geteax(void)
{
    return static_cast<_fix_16>(eax.GetValue());
}

_fix_16 getsay(void)
{
    return static_cast<_fix_16>(say.GetValue());
}

_fix_16 geteay(void)
{
    return static_cast<_fix_16>(eay.GetValue());
}

void InitializeDSPCore(void)
{
    static bool bInitialized = false;
    if (!bInitialized)
    {   
        a0.SetPart(&r0, 0, 16);
        a0.SetPart(&r1, 1, 16);
        a0.SetPart(&o0, 2, 8);

        a1.SetPart(&r2, 0, 16);
        a1.SetPart(&r3, 1, 16);
        a1.SetPart(&o1, 2, 8);
    
        a2.SetPart(&r4, 0, 16);
        a2.SetPart(&r5, 1, 16);
        a2.SetPart(&o2, 2, 8);
    
        a3.SetPart(&r6, 0, 16);
        a3.SetPart(&r7, 1, 16);
        a3.SetPart(&o3, 2, 8);

        o1o0.SetPart(&o0, 0, 8);
        o1o0.SetPart(&o1, 1, 8);
        o3o2.SetPart(&o2, 0, 8);
        o3o2.SetPart(&o3, 1, 8);

        p0h.SetProductLowObject(&p0l);
        p1h.SetProductLowObject(&p1l);

        p0.SetPart(&p0l, 0, 16);
        p0.SetPart(&p0h, 1, 16);
        p0.SetPart(&p0o, 2, 8);

        p1.SetPart(&p1l, 0, 16);
        p1.SetPart(&p1h, 1, 16);
        p1.SetPart(&p1o, 2, 8);

        y0x0.SetPart(&x0, 0, 16);
        y0x0.SetPart(&y0, 1, 16);
        y1x1.SetPart(&x1, 0, 16);
        y1x1.SetPart(&y1, 0, 16);

        lean.Coorperators[0] = &lcn;
        lean.Coorperators[1] = &lsa;
         lcn.Coorperators[0] = &lean;
         lcn.Coorperators[1] = &lsa;   
     
        px0.SetPart(&pxm0, 0);
        px0.SetPart(&pxa0, 1);
        px0.SetPart(&pxs0, 2);
        px1.SetPart(&pxm1, 0);
        px1.SetPart(&pxa1, 1);
        px1.SetPart(&pxs1, 2);
        py0.SetPart(&pym0, 0);
        py0.SetPart(&pya0, 1);
        py0.SetPart(&pys0, 2);
        py1.SetPart(&pym1, 0);
        py1.SetPart(&pya1, 1);
        py1.SetPart(&pys1, 2);

        sx0.SetPart(&sxm0, 0);
        sx1.SetPart(&sxm1, 0);      
        sy0.SetPart(&sym0, 0);      
        sy1.SetPart(&sym1, 0);      
    
        sx0.SetPart(&sxm0, 1);
        sx1.SetPart(&sxm1, 1);
        sy0.SetPart(&sym0, 1);
        sy1.SetPart(&sym1, 1);
    
        sx0.SetPart(&sxa0, 2);
        sx1.SetPart(&sxa1, 2);
        sy0.SetPart(&sya0, 2);
        sy1.SetPart(&sya1, 2);
                      
        sax.SetPart(&saxm, 0);       
        eax.SetPart(&eaxm, 0);       
        say.SetPart(&saym, 0);       
        eay.SetPart(&eaym, 0);
           
        sax.SetPart(&saxa, 2); 
        eax.SetPart(&eaxa, 2); 
        say.SetPart(&saya, 2); 
        eay.SetPart(&eaya, 2);

        bInitialized = true;

        printf("================ REAL DSP CORE INITIALIZED ================\n");
        gREALObjectPool.ShowInfo();
        printf("===========================================================\n");
    }

    saxm = 0;
    saxa = 0;
    eaxm = 0xFFFF;
    eaxa = 0xFFFF;

    saym = 0;
    saya = 0;
    eaym = 0xFFFF;
    eaya = 0xFFFF;

    sr = 0;
    cr = 0;
}

void ResetDSPCore(void)
{
    gREALObjectPool.Reset();
    gREALMemoryPool.Reset();
    InitializeDSPCore();
}