                                  
#ifndef _DSP_CPP_H

#define _DSP_CPP_H

#if !defined(__cplusplus)
    #error "C++ compiler required."
#endif

#ifdef _MSC_VER

    // warning C4127: conditional expression is constant
    #pragma warning(disable: 4127)

    // warning C4514: unreferenced inline function has been removed
    #pragma warning(disable: 4514)

    // since there might be many unreferenced labels, it's better to disable the warning 4102
    #pragma warning(once: 4102)

    #define for if (0) ; else for
    
#else
    #warning "Warning: this FrameWork has only been tested under Microsoft C++ Compiler 6.0"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "DSPTypes.h"
#include "DSPIO.h"

#define MAX_NAME    10
#define MAX_PARTS   5
#define MAX_X       0xFFFF
#define MAX_Y       0x7FFF
#define MAX_P       0xFFFF
#define STACK_DEPTH 5

#define XAllocated  0x0001U
#define YAllocated  0x0002U
#define PAllocated  0x0004U
#define XYAllocated 0x0003U

#define MAX_TRY_TIMES 5

#define DEFAULT_SCHEME masAuto //masContinouse // masAuto

// since rand() is in [0, 0x7FFF], here we make an improvement
#define random() (rand() << 17 | rand() << 1 | rand() & 0x0001)

enum CMemClass
{
    XMem  = XAllocated,
    YMem  = YAllocated,
    XYMem = XYAllocated,
    PMem  = PAllocated
};

enum CMemoryAllocationScheme
{
    masAuto,
    masContinouse,
    masManual
};

typedef int (__stdcall * PMessageBox) (void *Handle, const char *Text, const char *Caption, const int nID);
//PMessageBox MessageBox2 = NULL;

void InitializeDSPCore(void);
void ResetDSPCore(void);

// Control and Status functions
int getam (void);
int getgie(void);
int getlof(void);
int getz  (void);
int getof (void);
int getca (void);
int getn  (void);
int getodd(void);
int getac (void);
int getlnz(void);
int setlof(const bool flag);
int setz  (const bool flag);
int setof (const bool flag);
int setc  (const bool flag);
int setn  (const bool flag);
int setodd(const bool flag);
int setac (const bool flag);
int setlnz(const bool flag);
int getpss(void);
int getprm(void);
int getsat(void);
int getbsm(void);
int getnie(void);
int getsa (void);
int getsb (void);
int getbr (void);
int getpage(void);

bool   getparallel(void);
DWORD  getsig(void);
void enterparallel(void);
void leaveparallel(void);

template <class _T> _fix_40 setstatus(const _T res, _fix_40 fullres = 0)
{
    setz(res == 0);
    setn(res < 0);
 
    if (sizeof(_T) == sizeof(_fix_16))
    {
        setodd((res & 1) != 0);         // odd only be change in 16bit operation
        setof((fullres > 0x7FFF) || (fullres < -0x8000));        
        setc((fullres & 0x10000) != 0);
    }
    else if (sizeof(_T) == sizeof(_fix_40))
    {
        setof((fullres > 0x7FFFFFFF) || (fullres < -_fix_40(0x80000000)));
        setac((fullres & 0x10000000000) != 0);
        setc((fullres & 0xFF00000000) != 0);
    }

    return fullres != 0 ? fullres : res;
};

template <class _T>_T stu(_fix_40 a)
{
    if (getsat())
    {    
        if (sizeof(_T) == sizeof(_fix_40))
        {        
            _fix_40 res = a;
            if (a > 0x7FFFFFFF)    
                res = 0x7FFFFFFF;
            else if (a < -_fix_40(0x80000000))
                res = 0x80000000;
            else;
            setz(res == 0);
            setn(res < 0);
            return static_cast<_T>(res);
        }
        else if (sizeof(_T) == sizeof(_fix_16))
        {
            _fix_40 res = a;
            if (a > 0x7FFF)    
                res = 0x7FFF;
            else if (a < -_fix_40(0x8000))
                res = 0x8000;
            else;
            setz(res == 0);
            setn(res < 0);
            return static_cast<_T>(res);
        }
        else
            return static_cast<_T>(a);
    }    
    else
        return static_cast<_T>(a);
};

template <class _T> _T fixand(_T a, _T b)
{
    return stu<_T>(setstatus<_T>(static_cast<_T>(a & b)));
};

template <class _T> _T fixor(_T a, _T b)
{
    return stu<_T>(setstatus<_T>(static_cast<_T>(a | b)));
};

template <class _T> _T fixxor(_T a, _T b)
{
    return stu<_T>(setstatus<_T>(static_cast<_T>(a ^ b)));
};

template <class _T> _T fixpass(_T a)
{
    return stu<_T>(setstatus<_T>(a));
};

template <class _T> _T fixnot(_T a)
{
    return stu<_T>(setstatus<_T>(~a));
};

template <class _T> _T fixplus(_T a, _T b, _T c = 0)
{
    setstatus<_T>(static_cast<_T>(a + b + c), static_cast<_fix_40>(a) + b + c);
    return stu<_T>(static_cast<_fix_40>(a) + b + c);
};

template <class _T> _T fixsub(_T a, _T b, _T q = 0)
{
    _T res = stu<_T>(setstatus<_T>(static_cast<_T>(a -b - q), static_cast<_fix_40>(a) -b - q));
    setc(!getca());
    return res;
};

template <class _T> _T fixmax(_T a, _T b)
{
    _T res = (a > b) ? a : b;
    setstatus<_T>(res, static_cast<_fix_40>(a) > b ? a : b);
    setc(a >= b);
    return stu<_T>(res);
};

_fix_16 fixdiv(_fix_16 & r0, _fix_16 & x0, _fix_16 & dr);

template <class _T> _T fixabs(_T a)
{
    return stu<_T>(setstatus<_T>(a >= 0 ? a : -a));
};

template <class _T> _T fixlsh(_T a, _T b)
{
    if (b > 0)
        return stu<_T>(setstatus<_T>(static_cast<_T>(a << b), static_cast<_fix_40>(a) << b));
    else
        return stu<_T>(setstatus<_T>(static_cast<_T>(a >> -b), static_cast<_fix_40>(a) << b));
};

template <class _T> _T fixrsh(_T a, _T b)
{
    if (b > 0)
        return stu<_T>(setstatus<_T>(static_cast<_T>(a >> b), static_cast<_fix_40>(a) >> b));
    else
        return stu<_T>(setstatus<_T>(static_cast<_T>(a << -b), static_cast<_fix_40>(a) >> b));
};

template <class _T> _T fixrls(_T a)
{
    if (sizeof(_T) == sizeof(_fix_16))
    {
        _fix_16 res = ((a & 0x7FFF) << 1) + getca();
        setstatus<_T>(res);
        setc((a & 0x8000) != 0);
        return stu<_T>(res);
    }
    else
    {
        dspprint("ERROR: '<<< 1' only valid to _fix_16\n");
        return a;
    };
};

template <class _T> _T fixrrs(_T a)
{
    if (sizeof(_T) == sizeof(_fix_16))
    {
        _fix_16 res = ((a & 0xFFFE) >> 1) | getca() << 15;
        setstatus<_T>(res);
        setc((a & 0x0001) != 0);
        return stu<_T>(res);
    }
    else
    {
        dspprint("ERROR: '>>> 1' only valid to _fix_16\n");
        return a;
    };
};

template <class _T> _T fixdiv2(_T a)
{
    return stu<_T>(setstatus<_T>(static_cast<_T>(a / 2)));
};

template <class _T> _fix_40 fixtimes(_T a, _T b)
{
    _fix_40 res = static_cast<_fix_40>(a) * b;
    if (getpss() == 1)
        res <<= 1;
    else if (getpss() == 2)
    {
        res <<= 1;
        res = stu<_fix_40>(res);
    }
    else if (getpss() == 3)
        res <<= 2;
    else;

    return res;
}

// round 40bit t0 24bit
_fix_40 fixround(_fix_40 a);
_fix_40 fixsat(_fix_40 a);
_fix_16 fixnorm(_fix_40 a);

char * PrintInt(char *str, const _fix_40 value, const int BitLen);
class  CDSPObject;
int    RegisterObject(CDSPObject *pObj);

/*
_fix_16 ReadMemory(const int Address, const CMemClass MemClass);
void    WriteMemory(const int Address, const _fix_16 Value, const CMemClass MemClass);
*/

class CDSPProductHighObject;
bool getprpadding(void);
void setprpadding(const bool b);

void setph(CDSPProductHighObject *p);
CDSPProductHighObject *getph(void);

class CDSPObject
{
friend class CDSPMemoryPool;

private:
    enum TObjectAttribute
    {
        oaInitialized       = 0x0001,
        oaDelayedSet        = 0x0002,
        oaParallelizable    = 0x0004,
        oaWriteProtect      = 0x0008,
        oaReadProtect       = 0x0010,
    };

    bool GetAttribute(const TObjectAttribute Attrib) const
    {
        return (Attribute & Attrib) != 0;
    }

    void SetAttribute(const TObjectAttribute Attrib, const bool Value)
    {
        if (Value)
            Attribute |= Attrib;
        else
            Attribute &= ~Attrib;         
    }

protected:
    WORD    Attribute;
    DWORD   ParalleleSig;
public:
    char    Name[MAX_NAME + 1];

protected:
    virtual _fix_40            _GetValue(const bool = 0) const
    {   
        return 0;
    }

    virtual const CDSPObject & _SetValue(const _fix_40, const bool bDelayed)
    {
        SetAttribute(oaInitialized, true);        
        if (GetAttribute(oaDelayedSet) && bDelayed) 
        {
            dspprint("FATAL: '%s' cannot be driven by more than one sources on the same clock edge\n", GetName());
        }   
        SetAttribute(oaDelayedSet, bDelayed);        
        ParalleleSig = getsig();
        return *this;
    }

public:
    CDSPObject(const CDSPObject &)
    {
        dspprint("INTERNAL FATAL: copy construction of CDSPObject is not allowed\n");
    };

    CDSPObject(const char * name, const bool Parallelizable = true)
    {                
        if (strlen(name) <= MAX_NAME)
            strcpy(Name, name);
        else
        {
            dspprint("INTERNAL FATAL: CDSPObject::CDSPObject() name too long '%s'", name);
            Name[0] = '\0';
        }

        Attribute       = 0;        
        ParalleleSig    = 0;

        SetAttribute(oaParallelizable, Parallelizable);

        if ((strlen(Name) > 0) && (strcmp(Name, "temp") != 0))
            RegisterObject(this);
        else;
    };

public:
    virtual _fix_40 GetValue(void) const
    {
        if (!IsInitialized())
            dspprint("WARNING: '%s' is read before initialization\n", GetName());
        else;
        if ((!getparallel() || getparallel() && (ParalleleSig != getsig())) && GetAttribute(oaDelayedSet))
        {
            const_cast<CDSPObject *>(this)->_SetValue(_GetValue(true), false);
        }
        return _GetValue();
    }

    virtual void Reset(void)
    {
        SetAttribute(oaInitialized, false);    
        SetAttribute(oaDelayedSet, false); 
    }
        
    virtual void Print(void) const
    {
        int v = static_cast<int>(GetValue());
        dspprint("%-3s = 0x%04X = %d\n", GetName(), v, v);
    }

    virtual const CDSPObject & SetValue(const _fix_40 Value)
    {
        if (!IsReadOnly())
        {
            if ((!getparallel() || getparallel() && (ParalleleSig != getsig())) && GetAttribute(oaDelayedSet))
            {
                const_cast<CDSPObject *>(this)->_SetValue(_GetValue(true), false);
            }
            return _SetValue(Value, getparallel() && GetAttribute(oaParallelizable));
        }        
        else
        {
            dspprint("EXCEPTION: '%s' is READ-ONLY, write access denied\n", GetName());
            return *this;
        }
    }

public:
    virtual const char *GetName(void) const
    {
        return Name;
    }    
    
    virtual bool IsInitialized(void) const
    {
        return GetAttribute(oaInitialized);
    }
    
    virtual bool IsReadOnly(void) const
    {
        return GetAttribute(oaWriteProtect);
    }

    virtual void SetReadOnly(const bool Value)
    {
        SetAttribute(oaWriteProtect, Value);
    }

    virtual void Initialize(void)
    {
        SetValue(0);
    }

public:
    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return SetValue(value);
    }

    const CDSPObject & operator = (const CDSPObject & rhs)
    {
        return SetValue(rhs.GetValue());
    }

    const CDSPObject & operator += (const int increament)
    {
        return SetValue(GetValue() + increament);
    }

    const CDSPObject & operator -= (const int increament)
    {
        return SetValue(GetValue() - increament);
    }

    virtual const _fix_40      operator + (const CDSPObject & rhs)
    {
        return fixplus<_fix_40>(static_cast<_fix_16>(GetValue()), static_cast<_fix_16>(rhs.GetValue()));
    }

    virtual const _fix_40      operator + (const int increament)
    {
        return fixplus<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(increament));
    }

    virtual const _fix_40      operator - (const CDSPObject & rhs)
    {
        return fixsub<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    virtual const _fix_40      operator - (const int increament)
    {
        return fixsub<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(increament));
    }

    virtual const _fix_40      operator * (const CDSPObject & rhs)
    {
        return fixtimes<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    virtual const _fix_40      operator / (const int divisor)
    {
        if (divisor == 2)
            return fixdiv2<_fix_40>(static_cast<_fix_40>(GetValue()));
        else
        {
            dspprint("ERROR: CDSPRegister<_fix_40>::'/%d' is not supported\n", divisor);
            return 0;
        }
    }
    
    virtual const _fix_40      operator & (const CDSPObject & rhs)
    {
        return fixand<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    virtual const _fix_40      operator | (const CDSPObject & rhs)
    {
        return fixor<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    virtual const _fix_40      operator ^ (const CDSPObject & rhs)
    {
        return fixxor<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

/*
    virtual const _fix_40      operator <<(const CDSPObject & rhs)
    {
        return fixlsh<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    virtual const _fix_40      operator <<(const int shift)
    {
        return fixlsh<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(shift));
    }

    virtual const _fix_40      operator >>(const CDSPObject & rhs)
    {
        return fixrsh<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    virtual const _fix_40      operator >>(const int shift)
    {
        return fixrsh<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(shift));
    }
    
    virtual const _fix_40      operator + (const CDSPObject & rhs);
    virtual const _fix_40      operator + (const int increament);
    virtual const _fix_40      operator - (const CDSPObject & rhs);
    virtual const _fix_40      operator - (const int increament);
    virtual const _fix_40      operator * (const CDSPObject & rhs);    
    virtual const _fix_40      operator / (const int divisor);
    virtual const _fix_40      operator & (const CDSPObject & rhs);
    virtual const _fix_40      operator | (const CDSPObject & rhs);
    virtual const _fix_40      operator ^ (const CDSPObject & rhs);

    virtual const _fix_40      operator <<(const CDSPObject & rhs);
    virtual const _fix_40      operator <<(const int shift);
    virtual const _fix_40      operator >>(const CDSPObject & rhs);
    virtual const _fix_40      operator >>(const int shift);
*/
    virtual operator _fix_40 (void)
    {
        return GetValue();
    }

//    virtual const _fix_40       operator ++ (void);
//    virtual const _fix_40       operator -- (void);
};

class CMagicPointerRegister;

template <class _T> class CDSPRegister : public CDSPObject
{
protected:
    _T DelayedValue;
    _T Value;    

protected:
    virtual _fix_40 _GetValue(const bool bDelayed = false) const
    {
        CDSPObject::_GetValue(bDelayed);
        if (bDelayed)
            return DelayedValue;
        else
            return Value;
    }

    virtual const CDSPObject & _SetValue(const _fix_40 Value, const bool bDelayed)
    {        
        CDSPObject::_SetValue(Value, bDelayed);
        if (IsInitialized())
        {
            if (bDelayed)
                this->DelayedValue = static_cast<_T>(Value);
            else
                this->Value        = static_cast<_T>(Value);
        }
        else
            this->Value        = static_cast<_T>(Value);
        
        return *this;
    }

public:
    CDSPRegister(const bool Parallelizable = true) : CDSPObject("", Parallelizable)
    {
        DelayedValue = Value = static_cast<_T>(random());        
    }

    CDSPRegister(const char * name, const bool Parallelizable = true) : CDSPObject(name, Parallelizable)
    {
        DelayedValue = Value = static_cast<_T>(random());       
    }
/*       
    const CDSPObject & operator = (const CDSPRegister & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
*/
    virtual void Print(void) const
    {
        char str[40];
        str[0] = '\0';
        _fix_40 v = GetValue();
        dspprint("%-3s = 0x%s = %d\n", GetName(), PrintInt(str, v, sizeof(_T) * 8), v);
    }    

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }    

    const CDSPObject & operator = (const CDSPRegister & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    } 

    virtual const _fix_40      operator + (const CDSPObject & rhs)
    {
        return fixplus<_fix_40>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator + (const int increament)
    {
        return fixplus<_fix_40>(static_cast<_T>(GetValue()), static_cast<_T>(increament));
    }

    virtual const _fix_40      operator - (const CDSPObject & rhs)
    {
        return fixsub<_fix_40>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator - (const int increament)
    {
        return fixsub<_fix_40>(static_cast<_T>(GetValue()), static_cast<_T>(increament));
    }

    virtual const _fix_40      operator * (const CDSPObject & rhs)
    {
        return fixtimes<_fix_40>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator / (const int divisor)
    {
        if (divisor == 2)
            return fixdiv2<_fix_40>(static_cast<_fix_40>(GetValue()));
        else
        {
            dspprint("ERROR: CDSPRegister<_fix_40>::'/%d' is not supported\n", divisor);
            return 0;
        }
    }
    
    virtual const _fix_40      operator & (const CDSPObject & rhs)
    {
        return fixand<_fix_40>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator | (const CDSPObject & rhs)
    {
        return fixor<_fix_40>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator ^ (const CDSPObject & rhs)
    {
        return fixxor<_fix_40>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }
 
/*    
    virtual const _fix_40      operator + (const CDSPObject & rhs)
    {
        return fixplus<_T>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }
    
    virtual const _fix_40      operator + (const int increament)
    {
        return fixplus<_T>(static_cast<_T>(GetValue()), static_cast<_T>(increament));
    }

    virtual const _fix_40      operator - (const CDSPObject & rhs)
    {
        return fixsub<_T>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator - (const int increament)
    {
        return fixsub<_T>(static_cast<_T>(GetValue()), static_cast<_T>(increament));
    }

    virtual const _fix_40      operator * (const CDSPObject & rhs)
    {
        return fixtimes<_T>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator / (const int divisor)
    {
        if (divisor == 2)
            return fixdiv2<_T>(static_cast<_T>(GetValue()));
        else
        {
            dspprint("ERROR: CDSPRegister<_T>::'/%d' is not supported\n", divisor);
            return 0;
        }
    }

    virtual const _fix_40      operator & (const CDSPObject & rhs)
    {
        return fixand<_T>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator | (const CDSPObject & rhs)
    {
        return fixor<_T>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator ^ (const CDSPObject & rhs)
    {
        return fixxor<_T>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }
*/
    virtual const _fix_40      operator -(void)
    {
        return fixsub<_T>(0, static_cast<_T>(static_cast<_T>(this->GetValue())));
    }

    CDSPRegister<_T> & operator ++ (void)
    {
        SetValue(fixplus<_T>(static_cast<_T>(GetValue()), 1, 0));
        return *this;
    }
    
    CDSPRegister<_T> & operator -- (void)
    {
        SetValue(fixsub<_T>(static_cast<_T>(GetValue()), 1, 0));
        return *this;
    }
};

template <class _T> class CDSPDCURegister : public CDSPRegister<_T>
{
public:
    CDSPDCURegister(const bool Parallelizable = true) : CDSPRegister<_T>(Parallelizable)
    {       
    }

    CDSPDCURegister(const char * name, const bool Parallelizable = true) : CDSPRegister<_T>(name, Parallelizable)
    {     
    }

public:
    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }    

    const CDSPObject & operator = (const CDSPRegister<_T> & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    } 

    const _fix_40      operator <<(const CDSPObject & rhs)
    {
        return operator <<(static_cast<int>(rhs.GetValue()));
    }

    const _fix_40      operator <<(const int shift)
    {
        // barrel shift are always 32-bit
        _fix_40 res = static_cast<_fix_40>(this->GetValue());
        if (sizeof(_T) == sizeof(_fix_16))
            res <<= 16;
        return fixlsh<_fix_40>(res, static_cast<_fix_40>(shift));
    }

    const _fix_40      operator >>(const CDSPObject & rhs)
    {
        return operator >>(static_cast<int>(rhs.GetValue()));
    }

    const _fix_40      operator >>(const int shift)
    {
        // barrel shift are always 32-bit
        _fix_40 res = static_cast<_fix_40>(this->GetValue());
        if (sizeof(_T) == sizeof(_fix_16))
            res <<= 16;
        return fixrsh<_fix_40>(res, static_cast<_fix_40>(shift));
    }
/*
    virtual const _fix_40      operator <<(const CDSPObject & rhs)
    {
        return fixlsh<_T>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }

    virtual const _fix_40      operator <<(const int shift)
    {
        return fixlsh<_T>(static_cast<_T>(GetValue()), static_cast<_T>(shift));
    }

    virtual const _fix_40      operator >>(const CDSPObject & rhs)
    {
        return fixrsh<_T>(static_cast<_T>(GetValue()), static_cast<_T>(rhs.GetValue()));
    }
    
    virtual const _fix_40      operator >>(const int shift)
    {
        return fixrsh<_T>(static_cast<_T>(GetValue()), static_cast<_T>(shift));
    } 
*/    
};

typedef CDSPDCURegister<_fix_16> CDSP16BitDCURegister;

typedef CDSPRegister<_fix_8 > CDSP8BitRegister;
typedef CDSPRegister<_fix_16> CDSP16BitRegister;
typedef CDSPRegister<_fix_16> CDSP32BitRegister;
typedef CDSPRegister<_fix_40> CDSP40BitRegister;

typedef CDSPRegister<_ufix_16> CDSPUnsigned16BitRegister;

typedef CDSP16BitRegister     CDSPMemoryObject;

// Global Functions
CDSPMemoryObject & GetMemoryObject(const int Address, const CMemClass MemClass);

/*
class CDSPMemoryObject : public CDSP16BitRegister
{
public:
    CDSPMemoryObject()
        : CDSP16BitRegister("")
    {
    }

    CDSPMemoryObject(const char *name)
        : CDSP16BitRegister(name)
    {
    }

    const CDSPObject & operator = (const CDSPObject & rhs);
    const CDSPObject & operator = (const _fix_40 value);
    CDSPMemoryObject & operator * (void)
    {
        return *this;
    }
};
*/

class CDSPProductRoundFlag;
class CDSPProductHighObject;

class CDSPProductHighObject : public CDSPRegister<_fix_16>
{
private:
    CDSP16BitRegister *m_pLow;

public:
    CDSPProductHighObject(const char *name)
        : CDSPRegister<_fix_16>(name)
    {
        m_pLow = NULL;
    }

    virtual _fix_40 GetValue(void) const
    {
        setph(const_cast<CDSPProductHighObject *>(this));
        setprpadding(getbsm() != 0);
        return CDSPRegister<_fix_16>::GetValue();
    }
    
    int GetQ(void) const
    {
        if (NULL != m_pLow)
            return m_pLow->GetValue() < 0 ? 1 : 0;
        else
        {
            dspprint("INTERNAL ERROR: CDSPProductHighObject::operator +() no lower part\n");    
            return 0;
        }
    }

    void SetProductLowObject(CDSP16BitRegister *p)
    {
        m_pLow = p;
    }
};

class CDSPProductRoundFlag
{
public:
    operator int() const
    {
        int q = 0;
        if (getph() != NULL)
            q = getph()->GetQ();
        else
            dspprint("FATAL ERROR: q:: there is no recent p*h operation.\n");    

        setprpadding(false);
        setph(NULL);
        return q;
    }
};

class CDSPBitObject : public CDSPObject
{
private:
    CDSPObject *pOwner;

    __int64 ShiftedMask;
    __int64 Mask;
    int     Shift;
    int     BitLen;
public:
    friend class CDSPControlRegister;
    friend class CDSPStatusRegister;
    friend class CDSPDrRegister;
    CDSPBitObject(const char *name,
                  CDSPObject *pOwner, 
                  const int BitPos,     // zero-based
                  const int BitLen) : CDSPObject(name, false)
    {
        Shift = BitPos;
        ShiftedMask = ((1 << (BitPos + BitLen)) - 1) & (~((1 << BitPos) - 1));
        Mask        =  (1 << BitLen) - 1;
        this->pOwner = pOwner;
        this->BitLen = BitLen;
    }
    
    virtual void Print(void) const
    {
        char str[10];
        _fix_40 v = GetValue();
        PrintInt(str, v, BitLen);

        dspprint("%-3s = 0x%s\n", GetName(), str);
    }

    virtual _fix_40 GetValue(void) const
    {
        if (NULL == pOwner)
        {
            dspprint("INTERNAL ERROR: CDSPBitObject::GetValue() no owner\n");
            return 0;
        }

        // sign extension
        _fix_40 res = (pOwner->GetValue() & ShiftedMask) >> Shift;
        if (res & (1 << (BitLen - 1)))
            res |= (-1 << BitLen);
        else;
        return res;
    }

    virtual const CDSPObject & SetValue(const _fix_40 Value)
    {        
        if (NULL == pOwner)
        {
            dspprint("INTERNAL ERROR: CDSPBitObject::SetValue() no owner\n");
            return *this;
        }

        _fix_40 temp  =  pOwner->GetValue() & (~ShiftedMask);
        temp         |= (Value & Mask) << Shift;
        
        return pOwner->SetValue(temp);
    }

    virtual bool IsInitialized(void) const
    {
        if (NULL == pOwner)
            return false;
        else
            return pOwner->IsInitialized();
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPBitObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
/*
    const CDSPObject & operator = (const CDSPObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
*/
};

typedef void (* PBeforePop)(void);

template <class _T> class CDSPStackedObject : public _T
{
private:
    PBeforePop BeforePop;

public:
    CDSPStackedObject(const char *name, 
                      const bool bPushOnWrite = false,
                      const bool bPopOnRead = false,
                      const bool bClearOnPop = false,
                      const PBeforePop BeforePop = NULL)
                      : _T(name)
    {
        for (int i = 0; i < STACK_DEPTH; i++)
        {
            sprintf(Objects[i].Name, "%s%d", name, i);
            Objects[i] = 0;
        }
        Top = 0;
        this->bClearOnPop  = bClearOnPop;
        this->bPopOnRead   = bPopOnRead;
        this->bPushOnWrite = bPushOnWrite;
        this->BeforePop    = BeforePop;
        memset(Coorperators, 0, sizeof(Coorperators));
    }

    virtual _fix_40 GetValue(void) const
    {
        _fix_40 res = Objects[Top].GetValue();
        if (bPopOnRead)
            const_cast<CDSPStackedObject *>(this)->CustomPop();
        else;
        return res;
    }

    virtual const CDSPObject & SetValue(const _fix_40 Value)
    {        
        if (bPushOnWrite)
            CustomPush();
        else;
        return Objects[Top].SetValue(Value);
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPStackedObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }

    _T & TopObj(void)
    {
        return Objects[Top];
    }

    virtual void CustomPush()
    {
        Push();
        for (int i = 0; i <= MAX_PARTS; i++)
        {
            if (Coorperators[i] != NULL)
                Coorperators[i]->Push();
        }
    }

    virtual void CustomPop()
    {
        Pop();
        for (int i = 0; i <= MAX_PARTS; i++)
        {
            if (Coorperators[i] != NULL)
                Coorperators[i]->Pop();
        }
    }

    void Push()
    {
        if (Top >= STACK_DEPTH)
            dspprint("WARNING: CDSPStackedObject<T>::stack '%s' overflow\n", this->GetName());
        else;
        Top = (Top + 1) % STACK_DEPTH;
    }

    void Pop()
    {
        if (BeforePop != NULL)
            BeforePop();
        else;

        if (bClearOnPop)
            Objects[Top] = 0;
        else;
        if (Top <= 0)
            dspprint("WARNING: CDSPStackedObject<T>::stack '%s' underflow\n", this->GetName());
        else;
        Top = (Top - 1 + STACK_DEPTH) % STACK_DEPTH;
    }
protected:
    _T Objects[STACK_DEPTH];
public:
    CDSPStackedObject<_T> *Coorperators[MAX_PARTS + 1];
private:
    int  Top;
    bool bPushOnWrite;
    bool bPopOnRead;
    bool bClearOnPop;
};

class CDSPCombinedObject : public CDSPObject
{
private:
    struct PartObj
    {
        CDSPObject *pObj;
        int         BitLen;
        DWORD       Mask;
    };
    PartObj Parts[MAX_PARTS + 1];

/*
    void UpdateName(void)
    {
        Name
        for (int i = 0; i <= MAX_PARTS; i++)
        {
            if (NULL == Parts[i].pObj) 
                continue;
            else;
            result |= (Parts[i].pObj->GetValue() & Parts[i].Mask) << shift;
            shift  +=  Parts[i].BitLen;
        }
    }
*/
public:
    CDSPCombinedObject(const char *name) : CDSPObject(name)
    {
        memset(Parts, 0, sizeof(Parts));
    }

    int SetPart(CDSPObject *pObj, const int Index, const int BitLen)
    {
        if ((Index < 0) || (Index > MAX_PARTS))
            return -1;

        Parts[Index].pObj = pObj;
        Parts[Index].BitLen = BitLen;
        Parts[Index].Mask = (1 << BitLen) - 1;
        return Index;
    }
    
    virtual _fix_40 GetValue(void) const
    {
        _fix_40 result = 0;
        int     bFirst = true;
        for (int i = MAX_PARTS; i >= 0; i--)
        {
            if (NULL == Parts[i].pObj) 
                continue;
            else;
            if (!bFirst)
            {
                result = (result << Parts[i].BitLen) | (Parts[i].pObj->GetValue() & Parts[i].Mask);
            }
            else
            {
                if ((Parts[i].pObj->GetValue() & (1 << (Parts[i].BitLen - 1))) != 0)
                {
                    result = -1;
                    result =   (result << (Parts[i].BitLen - 1)) 
                             | (Parts[i].pObj->GetValue() & (Parts[i].Mask >> 1));
                }
                else
                    result = Parts[i].pObj->GetValue() & (Parts[i].Mask >> 1);
                bFirst = false;
            }            
        }

        return result;
    }

    virtual const CDSPObject & SetValue(const _fix_40 Value)
    {        
        _fix_40 temp  = Value;
        for (int i = 0; i <= MAX_PARTS; i++)
        {
            if (NULL == Parts[i].pObj) 
                continue;
            else;

            Parts[i].pObj->SetValue(temp & Parts[i].Mask);
            temp >>= Parts[i].BitLen;
        }
        
        return *this;
    }

    virtual bool IsInitialized(void) const
    {
        for (int i = 0; i <= MAX_PARTS; i++)
        {
            if ((Parts[i].pObj != NULL) && (!Parts[i].pObj->IsInitialized()))
                return false;
            else;
        }
        return true;            
    }

    virtual void Print(void) const
    {
        char str[MAX_PARTS * 4 + 1];
        char temp[10];
        str[0] = '\0';
        str[1] = '\0';
        for (int i = MAX_PARTS; i >= 0; i--)
        {
            if (NULL == Parts[i].pObj) 
                continue;
            else;

            strcat(strcat(str, PrintInt(temp, Parts[i].pObj->GetValue(), Parts[i].BitLen)), ",");
        }

        str[strlen(str + 1)] = '\0';
        dspprint("%-3s = 0x%s\n", GetName(), str);
    }

    const CDSPObject & operator = (const CDSP16BitRegister & rhs)
    {
        if (Parts[1].pObj != NULL) 
        {            
            if (Parts[2].pObj != NULL)
            {            
                // sign extension
                if ((rhs.GetValue() & 0x8000) != 0)
                    Parts[2].pObj->SetValue(-1);
                else
                    Parts[2].pObj->SetValue(0);
            }
            else;

            return Parts[1].pObj->SetValue(rhs.GetValue());
        }
        else
            return *reinterpret_cast<CDSPObject *>(NULL);
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPCombinedObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }

    const _fix_40      operator <<(const CDSPObject & rhs)
    {
        return fixlsh<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    const _fix_40      operator <<(const int shift)
    {
        return fixlsh<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(shift));
    }

    const _fix_40      operator >>(const CDSPObject & rhs)
    {
        return fixrsh<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    const _fix_40      operator >>(const int shift)
    {
        return fixrsh<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(shift));
    }

    virtual const _fix_40      operator + (const CDSPObject & rhs)
    {
        return fixplus<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(rhs.GetValue()));
    }

    virtual const _fix_40      operator + (const int increament)
    {
        return fixplus<_fix_40>(static_cast<_fix_40>(GetValue()), static_cast<_fix_40>(increament));
    }

/*
    const CDSPObject & operator = (const CDSPObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
*/
};

class CDSPControlRegister : public CDSPRegister<_fix_16>
{
public:
    CDSPBitObject pss;
    CDSPBitObject prm;
    CDSPBitObject sat;
    CDSPBitObject bsm;
    CDSPBitObject nie;
    CDSPBitObject sa;
    CDSPBitObject sb;
    CDSPBitObject br;
public:
    CDSPControlRegister() : CDSPRegister<_fix_16>("cr"), 
                            pss("cr.pss", NULL, 0, 2),                          
                            prm("cr.prm", NULL, 2,  1),                          
                            sat("cr.sat", NULL, 3,  1),                          
                            bsm("cr.bsm", NULL, 4,  1),                          
                            nie("cr.nie", NULL, 5,  1),                          
                            sa ("cr.sa ",  NULL, 11, 1),                          
                            sb ("cr.sb ",  NULL, 12, 1),       
                            br ("cr.br ",  NULL, 15, 1)  
    { 
        pss.pOwner = this;
        prm.pOwner = this;
        sat.pOwner = this;
        bsm.pOwner = this;
        nie.pOwner = this;
         sa.pOwner = this;
         sb.pOwner = this;
         br.pOwner = this;
    }

    virtual void Print(void) const
    {
        pss.Print();
        prm.Print();
        sat.Print();
        bsm.Print();
        nie.Print();
         sa.Print();
         sb.Print();
         br.Print(); 
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPControlRegister & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
};

class CDSPDrRegister : public CDSPRegister<_fix_16>
{
public:
    CDSPBitObject h;
    CDSPBitObject l;
public:
    CDSPDrRegister() : CDSPRegister<_fix_16>("dr"), 
                       l("dr.l", NULL, 0, 8),                          
                       h("dr.h", NULL, 8, 8)
    { 
        h.pOwner = this;
        l.pOwner = this;
    }

    virtual void Print(void) const
    {
        h.Print();
        l.Print();
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPDrRegister & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
/*
    const CDSPObject & operator = (const CDSPObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
*/
};

class CDSPStatusRegister : public CDSPRegister<_fix_16>
{
public:
    CDSPBitObject am;
    CDSPBitObject gie;
    CDSPBitObject nof;
    CDSPBitObject lof;
    CDSPBitObject z;
    CDSPBitObject of;
    CDSPBitObject c;
    CDSPBitObject n;
    CDSPBitObject odd;
    CDSPBitObject ac;
    CDSPBitObject lnz;
public:
    CDSPStatusRegister() : CDSPRegister<_fix_16>("sr", false), 
                             am("sr.am ",  NULL, 0,  2),                          
                            gie("sr.gie", NULL, 2,  1),                          
                            nof("sr.nof", NULL, 5,  1),                          
                            lof("sr.lof", NULL, 8,  1),                          
                              z("sr.z  ",   NULL, 9,  1),                          
                             of("sr.of ",  NULL, 10, 1),                          
                              c("sr.c  ",   NULL, 11, 1),  
                              n("sr.n  ",   NULL, 12, 1),                          
                            odd("sr.odd", NULL, 13, 1),                          
                             ac("sr.ac ",  NULL, 14, 1),
                            lnz("sr.lnz", NULL, 15, 1)  
    { 
         am.pOwner = this;                          
        gie.pOwner = this;                          
        nof.pOwner = this;                          
        lof.pOwner = this;                          
          z.pOwner = this;                          
         of.pOwner = this;                          
          c.pOwner = this;  
          n.pOwner = this;                          
        odd.pOwner = this;                          
         ac.pOwner = this;
        lnz.pOwner = this; 
    }

    virtual void Print(void) const
    {
         am.Print();                          
        gie.Print();                          
        nof.Print();                          
        lof.Print();                          
          z.Print();                          
         of.Print();                          
          c.Print();  
          n.Print();                          
        odd.Print();                          
         ac.Print();
        lnz.Print();
    }
/*
    virtual const CDSPObject & SetValue(const _fix_40 Value)
    {
        CDSPObject::SetValue(Value);
        if (am != 0)
        {
            Print();
        }
        return *this;
    }
*/    
    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPStatusRegister rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
/*
    const CDSPObject & operator = (const CDSPObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
*/
};

typedef _fix_16 (* PGetAddr)(void);

class CDSPPointerRegister : public CDSPUnsigned16BitRegister
{
private:
    CMemClass MemClass;
    PGetAddr  getsa;
    PGetAddr  getea;
public:
    CDSPPointerRegister(const CDSPPointerRegister &ref)
        : CDSPRegister<_ufix_16>("")
    {
//      dspprint("!!!!copy construction of CDSPPointerRegister: %s\n", GetName());
        if (NULL != &ref)
        {                
            MemClass = ref.MemClass;  
            getsa    = ref.getsa;
            getea    = ref.getea;
            strcpy(Name, "hang");               

            Initialize();
            Value = static_cast<_ufix_16>(ref.GetValue());
        }
    }

    CDSPPointerRegister(const char *name, const CMemClass MemClass, 
                        const PGetAddr getsa,
                        const PGetAddr getea) 
        : CDSPRegister<_ufix_16>(name)
    {
        this->MemClass = MemClass;
        this->getsa    = getsa;
        this->getea    = getea;
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPPointerRegister & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }

    virtual CDSPMemoryObject & operator * (void) const       // addressing
    {
        return GetMemoryObject(static_cast<int>(GetValue()), MemClass);
    }

    virtual CDSPMemoryObject & operator [](const int index) const   
    {
        return GetMemoryObject(static_cast<int>(GetValue()) + index, MemClass);
    }

    virtual const CDSPPointerRegister & operator --(void)
    {
        _fix_16 res = static_cast<_fix_16>(GetValue());
        if ((NULL != getsa) && (NULL != getea) && (getsa() == res))
            SetValue(getea());
        else
            SetValue(res - 1);
        return *this;
    }

    virtual const CDSPPointerRegister operator --(int)
    {
        CDSPPointerRegister result(*this);
        --*this;
        return result;
    }

    virtual const CDSPPointerRegister & operator ++(void)
    {
        _fix_16 res = static_cast<_fix_16>(GetValue());
        if ((NULL != getsa) && (NULL != getea) && (getea() == res))
            SetValue(getsa());
        else
            SetValue(res + 1);
        return *this;
    }

    virtual const CDSPPointerRegister operator ++(int)
    {
        CDSPPointerRegister result(*this);
        ++*this;
        return result;
    }
};

class CDSPRecordedPointerRegister : public CDSPPointerRegister
{
public:
    CDSPRecordedPointerRegister(const char *name, const CMemClass MemClass, 
                                const PGetAddr getsa,
                                const PGetAddr getea) 
        : CDSPPointerRegister(name, MemClass, getsa, getea)
    {
        MinValue = MaxValue = 0;
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPRecordedPointerRegister & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }

    int GetMinValue(void)
    {
        return MinValue;
    }
    
    int GetMaxValue(void)
    {
        return MaxValue;
    }

    void ShowInfo(void)
    {
        dspprint("%s: Min = 0x%04X, Max = 0x%04X, Range = %d\n", GetName(), MinValue, MaxValue, MaxValue - MinValue);
    }

    void ClearMinMax(void)
    {
        if (IsInitialized())
            MinValue = MaxValue = static_cast<int>(GetValue());
    }

protected:
    virtual const CDSPObject & _SetValue(const _fix_40 Value, const bool bDelayed)
    {   
        const bool bInited = IsInitialized();
        CDSPPointerRegister::_SetValue(Value, bDelayed);
        int v = static_cast<int>(GetValue());

        if (bInited)
        {
            if (v > MaxValue)
                MaxValue = v;
            else if (v < MinValue)
                MinValue = v;
            else;
        }        
        else
            MinValue = MaxValue = v;

        return *this;
    }

protected:
    int MinValue;
    int MaxValue;
};

class CDSPMemoryPool
{
private:
    struct MemoryBlock 
    {
        int          addr;
        int          size;
        MemoryBlock *pNext;
    };
protected:    
    CDSPMemoryObject Temporary;
    CDSPMemoryObject XMemory[MAX_X + 1];
    CDSPMemoryObject YMemory[MAX_Y + 1];
    CDSPMemoryObject PMemory[MAX_P + 1];
    WORD             AllocationTable[MAX_P + 1];

protected:
    MemoryBlock      *pXRoot;
    MemoryBlock      *pYRoot;
    MemoryBlock      *pPRoot;

    MemoryBlock * AddMemoryBlock(MemoryBlock *&pRoot, const int addr, const int size)
    {
        MemoryBlock *p = new MemoryBlock;
        p->addr = addr;
        p->size = size;
        p->pNext = pRoot != NULL ? pRoot->pNext : NULL;
        return pRoot = p;
    }

    // return size
    int RemoveMemoryBlock(MemoryBlock *&pRoot, const int addr)
    {
        if (NULL == pRoot)
            return -1;

        int res = 0;                
        if (addr == pRoot->addr)
        {
            res = pRoot->size;
            pRoot = pRoot->pNext;
            return res;
        }
        else;

        MemoryBlock *pNext = pRoot;
        while (pNext->pNext != NULL)
        {
            if (addr == pNext->pNext->addr)
            {
                res = pNext->pNext->size;
                pNext->pNext = pNext->pNext->pNext;
                return res;    
            }
            else
                pNext = pNext->pNext;
        }

        return 0;
    }

    // return size
    int FindMemoryBlockSize(MemoryBlock *pRoot, const int addr)
    {
        while (pRoot != NULL)
        {
            if (addr == pRoot->addr)
                return pRoot->size;
            else 
                pRoot = pRoot->pNext;
        }

        return 0;
    }

    // return size
    void RemoveAllMemoryBlock(MemoryBlock *&pRoot)
    {
        MemoryBlock *p = pRoot;
        pRoot = NULL;
        while (p != NULL)
        {
            MemoryBlock *temp = p;
            p = p->pNext;
            delete temp;
        }
    }

private:
    bool TryAllocateMemory(const int Address, const int Size, const CMemClass MemClass)
    {
        if ((Size <= 0) || (Address < 0) || (Address + Size - 1 < 0) || (Address + Size - 1> MAX_X))
            return false;
        else;

        WORD allocate = static_cast<WORD>(MemClass);

        switch (MemClass) {
        case XYMem:
            if (Address <= MAX_Y)
                return false;
        default:
            for (int i = Address; i < Address + Size; i++)
            {
                if ((AllocationTable[i] & allocate) != 0)
                    return false;
                else;
            }
        }

        return true;
    }

    bool CheckMemoryRange(const int Address,
                          const int Size, 
                          const CMemClass MemClass)
    {
        if ((Size <= 0) || (Address < 0))
            return false;
        else;
        switch (MemClass) 
        {
            case XMem:
                if (Address + Size - 1 > MAX_X)
                    return false;
        	    break;
            case YMem:
        	    if (Address + Size - 1 > MAX_Y)
                    return false;
        	    break;
            case XYMem:
        	    if ((Address <= MAX_Y) || (Address + Size - 1 > MAX_X))
                    return false;
        	    break;
            case PMem:
        	    if (Address + Size - 1 > MAX_P)
                    return false;
        	    break;
            default:
                return false;
        }
        return true;
    }

public:
    CDSPMemoryPool(void)
        : Temporary("temp")
    {
        for (int i = MAX_Y + 1; i <= MAX_X; i++)    
            sprintf(XMemory[i].Name, "xy@%04X", i);
        for (int i = 0; i <= MAX_Y; i++)  
        {
            sprintf(XMemory[i].Name, "x@%04X", i);
            sprintf(YMemory[i].Name, "y@%04X", i);
        }
        for (int i = 0; i <= MAX_P; i++)    
            sprintf(PMemory[i].Name, "p@%04X", i);
        memset(AllocationTable, 0, sizeof(AllocationTable));
        pXRoot = pYRoot = pPRoot = NULL;
        Temporary.SetReadOnly(true);
    }

    ~CDSPMemoryPool(void)
    {
        RemoveAllMemoryBlock(pXRoot);
        RemoveAllMemoryBlock(pYRoot);
        RemoveAllMemoryBlock(pPRoot);
    }
    
    int MemoryBlockLenAndRemove(const int Address, const CMemClass MemClass)
    {
        if ((MemClass == XMem) || (MemClass == XYMem))
            return RemoveMemoryBlock(pXRoot, Address);
        else if ((MemClass == YMem))
            return RemoveMemoryBlock(pYRoot, Address);
        else
            return RemoveMemoryBlock(pPRoot, Address);
    }

    int MemoryBlockLen(const int Address, const CMemClass MemClass)
    {
        if ((MemClass == XMem) || (MemClass == XYMem))
            return FindMemoryBlockSize(pXRoot, Address);
        else if ((MemClass == YMem))
            return FindMemoryBlockSize(pYRoot, Address);
        else
            return FindMemoryBlockSize(pPRoot, Address);
    }

    int FreeMemory(const int Address, const CMemClass MemClass, const int Size = 0)
    {
        if ((Address < 0) || (Address > MAX_X))
            return -1;
        else;
        
        DWORD clear = 0;
        if (XMem == MemClass)
            clear = ~XAllocated;
        else if (YMem == MemClass)
            clear = ~YAllocated;
        else if (PMem == MemClass)
            clear = ~PAllocated;
        else
            clear = ~XYAllocated;
        if ((AllocationTable[Address] & (~clear)) == 0)
        {
            dspprint("WARNING: CDSPMemoryPool():FreeMemory: memory not alloc yet @ 0x%04X type %d\n",
                   Address, MemClass);
            return -1;
        }
        else;
        
        int last =    Address - 1 + (Size > 0 ? Size : MemoryBlockLenAndRemove(Address, MemClass));  
        if ((last > MAX_X) || (last <= Address))
        {
            dspprint("ERROR: CDSPMemoryPool::FreeMemory() address out of range\n");
            return -1;
        }
        else;
        for (int i = Address; i <= last; i++)    
            AllocationTable[i] &= clear;
        return last - Address + 1;
    }

    int AllocateMemory(const CMemoryAllocationScheme Scheme, const int Size, const CMemClass MemClass, const int Address)
    {
        if (Size <= 0)
            return -1;

        bool succ = false;
        int  addr = 0;
        switch (Scheme) {
        case masAuto:
            for (int i = 0; i < MAX_TRY_TIMES; i++)
            {
                addr = random() & 0xFFFF;
                succ = TryAllocateMemory(addr, Size, MemClass);
                if (succ)
                    break;
                else;
            }
            if (succ)
                break;
            else;
        case masContinouse:
            addr = 0;
            while (addr <= MAX_X)
            {
                if ((AllocationTable[addr] & MemClass) == 0)
                {
                    succ = TryAllocateMemory(addr, Size, MemClass);
                    if (!succ)
                    {
                        while ((AllocationTable[addr++] & MemClass) == 0);
                    }
                    else
                        break;
                }
                else
                    addr++;
            }
        	break;
        case masManual:
            addr = Address;
            succ = TryAllocateMemory(addr, Size, MemClass);
            break;
        default:
            ;
        }

        if (succ)
        {
            if ((MemClass == XMem) || (MemClass == XYMem))
                AddMemoryBlock(pXRoot, addr, Size);
            else if ((MemClass == YMem))
                AddMemoryBlock(pYRoot, addr, Size);
            else
                AddMemoryBlock(pPRoot, addr, Size);
            
            for (int i = addr; i < addr + Size; i++)
                AllocationTable[i] |= MemClass;
            return addr;
        }
        else
        {
            dspprint("PROG FATAL: CDSPMemoryPool()::memory allocation failed:\n"
                     "            size = %d, type = %d\n", Size, MemClass);
            return -1;
        }
    }

    int Export(FILE *pf, const int Address, const int Size, const CMemClass MemClass)
    {
        if ((NULL == pf) || (Address < 0) || (Size <= 0) || (Address + Size - 1 > MAX_X))
            return -1;
        CDSPMemoryObject *p = NULL;
        if ((MemClass == XMem) || (MemClass == PMem) || (MemClass == XYMem))
        {
            if (MemClass == PMem)
                p = PMemory;
            else
                p = XMemory;
            for (int i = Address; i < Address + Size; i++)
            {
                fprintf(pf, "0x%04X,\n", (p[i].GetValue() & 0xFFFF));
            }
        }
        else if (MemClass == YMem)
        {
            int yend = Address + Size;
            if (yend > MAX_Y)
                yend = MAX_Y + 1;
            for (int i = Address; i < yend; i++)
                fprintf(pf, "0x%04X,\n", (YMemory[i].GetValue() & 0xFFFF));
            if (yend < Address + Size)
            {
                for (int i = yend; i < Address + Size; i++)
                    fprintf(pf, "0x%04X,\n", (XMemory[i].GetValue() & 0xFFFF));
            }            
        }
        
        return Size;
    }

    int Import(FILE *pf, const int Address, const int Size, const CMemClass MemClass)
    {
        if ((NULL == pf) || (Address < 0) || (Size <= 0) || (Address + Size - 1 > MAX_X))
            return -1;

        CDSPMemoryObject *p = NULL;
        if ((MemClass == XMem) || (MemClass == PMem) || (MemClass == XYMem))
        {
            if (MemClass == PMem)
                p = PMemory;
            else
                p = XMemory;
            for (int i = Address; i < Address + Size; i++)
            {
                int f = 0;
                fscanf(pf, "0x%X,\n", &f);
                p[i].SetValue(f);
            }
        }
        else if (MemClass == YMem)
        {
            int yend = Address + Size;
            if (yend > MAX_Y)
                yend = MAX_Y + 1;
            for (int i = Address; i < yend; i++)
            {
                int f = 0;
                fscanf(pf, "0x%X,\n", &f);
                YMemory[i].SetValue(f);
            }

            if (yend < Address + Size)
            {
                for (int i = yend; i < Address + Size; i++)
                {
                    int f = 0;
                    fscanf(pf, "0x%X,\n", &f);
                    YMemory[i].SetValue(f);
                }
            }            
        }
        
        return Size;
    }

    int CopyMemory(const WORD *pBuffer,                      
                   const int Size, 
                   const CMemClass MemClass, 
                   const int Address)
    {
        if (   (Address < 0) || (Size < 0) || (Address + Size - 1 > MAX_X) 
            || (MemClass == YMem) && (Address + Size - 1 > MAX_Y))
            return -1;

        switch (MemClass) 
        {
            case XMem:
            case XYMem:
                for (int i = Address; i < Address + Size; i++)
                {
                    XMemory[i] = pBuffer[i - Address];
                }
                break;
            case YMem:
                for (int i = Address; i < Address + Size; i++)
                {
                    YMemory[i] = pBuffer[i - Address];
                }
                break;
            default:
                for (int i = Address; i < Address + Size; i++)
                {
                    PMemory[i] = pBuffer[i - Address];
                }
        }

        return Size;
    }

    int MemoryMapping(const WORD *pBuffer,                      
                      const int Size, 
                      const CMemClass MemClass, 
                      const CMemoryAllocationScheme Scheme = DEFAULT_SCHEME, 
                      const int Address = 0)
    {
        int res = AllocateMemory(Scheme, Size, MemClass, Address);
        CopyMemory(pBuffer, Size, MemClass, res);
        return res;
    }
    
    int SetROMMask(const int Address,
                   const int Size, 
                   const CMemClass MemClass,
                   const bool Set_true_Clear_false)
    {
        if (!CheckMemoryRange(Address, Size, MemClass))
            return -1;
        for (int i = 0; i < Size; i++)
            GetMemoryObject(Address + i, MemClass).SetReadOnly(Set_true_Clear_false);
        return Size;
    }

    // Attention: this function may return a NULL reference.
    //            To avoid this, a temporary object is involved.
    CDSPMemoryObject & GetMemoryObject(const int Address, const CMemClass MemClass)
    {
        CDSPMemoryObject *presult = &Temporary;
        
        if ((Address < 0) || (Address > MAX_X))
            goto exit;
        
        switch (MemClass) {
        case XYMem:
            if (((AllocationTable[Address] & XYAllocated) != 0) && (Address > MAX_Y) && (Address <= MAX_X))
                presult = & XMemory[Address];
            else;
        	break;            
        case XMem:
            if (((AllocationTable[Address] & XAllocated) != 0) && (Address <= MAX_X))
                presult = & XMemory[Address];
            else;
        	break;
        case YMem:
            if (Address <= MAX_Y)
            {
                if ((AllocationTable[Address] & YAllocated) != 0)
                    presult = & YMemory[Address];
                else;
            }
            else if ((Address > MAX_Y) && (Address <= MAX_X))
            {
                if ((AllocationTable[Address] & XYAllocated) != 0)
                    presult = & XMemory[Address];
                else;
            }
        	break;
        default:
            if ((Address <= MAX_P) && ((AllocationTable[Address] & PAllocated) != 0))
                presult = & PMemory[Address];
            else;
        }
exit:
        if (&Temporary == presult)
        {
            const char msg[] = "EXCEPTION: CDSPMemoryPool()::access violation @ 0x%04X, type %d, DUMMY used \n";
            dspprint(msg, Address, MemClass);
            // dspprint("ERROR: CDSPMemoryPool()::address out of range @ 0x%04X, type %d\n", Address, MemClass);
        }

        return *presult;
    }

    void Reset(void)
    {
#define Reset_xxx(Obj, MAX) for (int i = 0; i <= MAX; i++) Obj[i].Reset()
        Reset_xxx(XMemory, MAX_X);
        Reset_xxx(YMemory, MAX_Y);
        Reset_xxx(PMemory, MAX_P);
#undef  Reset_xxx
        memset(AllocationTable, 0, sizeof(AllocationTable));
    }
};

#define REGISTER_COUNT 100
class CDSPObjectPool
{
private:
    int Next;
    CDSPObject *RegisterList[REGISTER_COUNT];
public:
    CDSPObjectPool(void)
    {
        Next = 0;
        memset(RegisterList, 0, sizeof(RegisterList));
    }
    
    int AddRegister(CDSPObject *pRegister)
    {
        if ((NULL == pRegister) || (Next >= REGISTER_COUNT))
            return -1;
        for (int i = 0; i < Next; i++)
        {
            if (strcmp(RegisterList[i]->GetName(), pRegister->GetName()) == 0)
                return i;
            else;
        }
        RegisterList[Next] = pRegister;
        return Next++;
    }

    void Reset(void)
    {
        for (int i = 0; i < Next; i++)
        {
            RegisterList[i]->Reset();
        }
    }

    int GetCount(void) const
    {
        return Next;
    }

    CDSPObject * GetObject(const int Index) const
    {
        if ((Index >= 0) && (Index < Next))
            return RegisterList[Index];
        else
            return NULL;
    }

    void ShowInfo(void)
    {
        dspprint("Totally %d registers installed.\n", GetCount());
    }
};

typedef int (* PGetObject)(void);

class CDSPMagicObject : public CDSPObject
{
public:
    CDSPMagicObject(const char *name, PGetObject GetObject)
        : CDSPObject(name), Temporary("temp")
    {
        this->GetObject = GetObject;
        memset(Parts, 0, sizeof(Parts));
    }

    void SetPart(CDSPObject *pObj, const int Index)
    {
        if ((Index >=0) && (Index <= MAX_PARTS))
            Parts[Index] = pObj;
    }
    
    virtual _fix_40 GetValue(void) const
    {        
        return GetPart().GetValue();
    }

    virtual const CDSPObject & SetValue(const _fix_40 Value)
    {        
        return const_cast<CDSPObject &>(GetPart()).SetValue(Value);
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPMagicObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
    
    virtual bool IsInitialized(void) const
    {
        return GetPart().IsInitialized();
    }

    CDSPObject * GetTheObject(void) const
    {
        return &const_cast<CDSPObject &>(GetPart());
    }
/*
    const CDSPObject & operator = (const CDSPObject & rhs)
    {
        return CDSPObject::operator = (rhs.GetValue());
    }
*/
protected:
    CDSPObject *Parts[MAX_PARTS + 1];
    CDSPObject Temporary;    
    PGetObject GetObject;

    const CDSPObject & GetPart(void) const
    {
        if (GetObject != NULL)
        {
            const int Index = GetObject();
            if ((Index >=0) && (Index <= MAX_PARTS))
            {
                if (Parts[Index] != NULL)                    
                    return *Parts[Index];
                else
                {
                    dspprint("INTERNAL FATAL: CDSPMagicObject::nothing selected by '%s' with index %d\n", GetName(), Index);
                    return Temporary;
                }
            }
            else
                return Temporary;
        }
        else
            return Temporary;
    }
};

class CDSPMagicPointerRegister : public CDSPMagicObject, public CDSPPointerRegister
{
public:
    CDSPMagicPointerRegister(const char *name, PGetObject GetObject, CMemClass MemClass)
        : CDSPMagicObject(name, GetObject), CDSPPointerRegister(name, MemClass, NULL, NULL)
    {
    }
    
    virtual void Print(void) const
    {
        CDSPPointerRegister::Print();
    }

    virtual _fix_40 GetValue(void) const
    {        
        return CDSPMagicObject::GetValue();
    }

    virtual const CDSPObject & SetValue(const _fix_40 Value)
    {        
        return CDSPMagicObject::SetValue(Value);
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPMagicObject::operator = (value);
    }

    const CDSPObject & operator = (const CDSPMagicPointerRegister & rhs)
    {
        return CDSPMagicObject::operator = (rhs.GetValue());
    }
   
    virtual CDSPMemoryObject & operator * (void) const
    {
        CDSPPointerRegister *p = dynamic_cast<CDSPPointerRegister *>(GetTheObject());        
        return (p != NULL) ? *(*p) : CDSPPointerRegister::operator *();
    }

    virtual const CDSPPointerRegister & operator --(void)
    {
        CDSPPointerRegister *p = dynamic_cast<CDSPPointerRegister *>(GetTheObject());        
        return (p != NULL) ? --(*p) : CDSPPointerRegister::operator --();
    }

    virtual const CDSPPointerRegister   operator --(int)
    {
        int dummy = 0; dummy;
        CDSPPointerRegister *p = dynamic_cast<CDSPPointerRegister *>(GetTheObject());        
        return (p != NULL) ? (*p)-- : CDSPPointerRegister::operator --(dummy);
    }

    virtual const CDSPPointerRegister & operator ++(void)
    {
        CDSPPointerRegister *p = dynamic_cast<CDSPPointerRegister *>(GetTheObject());        
        return (p != NULL) ? ++(*p) : CDSPPointerRegister::operator ++();
    }

    virtual const CDSPPointerRegister   operator ++(int)
    {
        int dummy = 0; dummy;
        CDSPPointerRegister *p = dynamic_cast<CDSPPointerRegister *>(GetTheObject());        
        return (p != NULL) ? (*p)++ : CDSPPointerRegister::operator ++(dummy);
    }
    
    /*
    operator CDSPObject &(void)
    {
        return static_cast<CDSPMagicObject &>(*this);
    }
    
    operator const CDSPObject &(void)
    {
        return static_cast<CDSPMagicObject &>(*this);
    }
    */

    operator _fix_40 (void) const
    {
        return GetValue();
    }
};

class CDSPMemAddress
{
public:
    CDSPMemAddress(const int addr, const CMemClass MemClass)
    {
        this->addr = addr;
        this->MemClass = MemClass;
    }

    operator int (void)
    {
        return addr;
    }

    CDSPMemoryObject & operator * (void)
    {
        return GetMemoryObject(addr, MemClass);
    }

    CDSPMemoryObject & operator [](const int offset)
    {
        return GetMemoryObject(addr + offset, MemClass);
    }

    CDSPMemAddress   operator + (const int offset)
    {
        return CDSPMemAddress(addr + offset, MemClass);
    }

    CDSPMemAddress   operator - (const int offset)
    {
        return CDSPMemAddress(addr - offset, MemClass);
    }

    CDSPMemAddress & operator ++(void)
    {
        addr++;
        return *this;
    }

    CDSPMemAddress operator ++(const int)
    {
        addr++;
        return CDSPMemAddress(addr - 1, MemClass);
    }

    CDSPMemAddress & operator --(void)
    {
        addr--;
        return *this;
    }

    CDSPMemAddress operator --(const int)
    {
        addr--;
        return CDSPMemAddress(addr + 1, MemClass);
    }

protected:
    int addr;
    CMemClass MemClass;
};

class CDSPXMemAddress : public CDSPMemAddress
{
public:
    CDSPXMemAddress(const int addr)
        : CDSPMemAddress(addr, XMem)
    {
    }

    CDSPXMemAddress(void)
        : CDSPMemAddress(0, XMem)
    {
    }
};

class CDSPYMemAddress : public CDSPMemAddress
{
public:
    CDSPYMemAddress(const int addr)
        : CDSPMemAddress(addr, YMem)
    {
    }

    CDSPYMemAddress(void)
        : CDSPMemAddress(0, YMem)
    {
    }
};

class CDSPPZRegister : public CDSPPointerRegister
{
public:
    CDSPPZRegister(const CDSPPZRegister & ref)
        : CDSPPointerRegister("temp", XMem, NULL, NULL)
    {
        if (this != &ref)
        {
            *this = ref;
        }
    }

    CDSPPZRegister(const char *name)
        : CDSPPointerRegister(name, XMem, NULL, NULL)
    {
    }

    virtual const CDSPObject & operator = (const _fix_40 value)
    {
        return CDSPObject::operator = (value);
    }

    virtual const CDSPObject & operator = (const CDSPPZRegister & rhs)
    {
        CDSPObject::operator = (rhs.GetValue());
        return *this;
    }

    const CDSPPZRegister operator >> (const int shift)
    {
        CDSPPZRegister res(*this);

        if (shift == 4)         // bit
            res = ((getpage() & 0xF) << 12) | ((GetValue() & 0xFFFF) >> 4);
        else if (shift == 2)    // nibble
            res = ((getpage() & 0xC) << 12) | ((GetValue() & 0xFFFF) >> 2);
        else if (shift == 1)    // byte
            res = ((getpage() & 0x8) << 12) | ((GetValue() & 0xFFFF) >> 1);
        else if (shift == 0)    // byte
            ;
        else
            dspprint("EXCEPTION: invalid instruction: '%s >> %d'", 
                     GetName(), shift);
        return res;
    }   

public:
    static _fix_16 GetBit(const CDSPObject &bmp, const CDSPPZRegister &pzp)
    {
        const int pz  = static_cast<int>(pzp.GetValue());
        const int bm  = static_cast<int>(bmp.GetValue());

        const int res = (bm >> (pz & 0xF)) & 0x1;
        setc(res != 0);
        return static_cast<_fix_16>(res != 0 ? 0xFFFF : 0x0000);
    }

    static _fix_16 GetNibble(const CDSPObject &bmp, const CDSPPZRegister &pzp)
    {
        const int pz  = static_cast<int>(pzp.GetValue());
        const int bm  = static_cast<int>(bmp.GetValue());

        const int res = (bm >> 4 * (pz & 0x3)) & 0xF;
        setc((res & 0x8) != 0);
        return static_cast<_fix_16>((res & 0x8) != 0 ? 0xFFF0 | res : res);
    }

    static _fix_16 GetCMI(const CDSPObject &bmp, const CDSPPZRegister &pzp)
    {
        const int pz  = static_cast<int>(pzp.GetValue());
        const int bm  = static_cast<int>(bmp.GetValue());

        const int res = ((bm >> 4 * (pz & 0x3)) & 0xF) * 2 + 1;   // << 1, then + 1
        setc((res & 0x10) != 0);
        return static_cast<_fix_16>((res & 0x10) != 0 ? 0xFFE0 | res : res);
    }

    static _fix_16 GetByte(const CDSPObject &bmp, const CDSPPZRegister &pzp)
    {
        const int pz  = static_cast<int>(pzp.GetValue());
        const int bm  = static_cast<int>(bmp.GetValue());

        const int res = (bm >> 8 * (pz & 0x1)) & 0xFF;
        setc((res & 0x80) != 0);
        return static_cast<_fix_16>((res & 0x80) != 0 ? 0xFF00 | res : res);
    }

    static _fix_16 PutBit(const CDSPObject &bmp, const CDSPPZRegister &pzp)
    {
        const int pz  = static_cast<int>(pzp.GetValue());
        const int bm  = static_cast<int>(bmp.GetValue());
        CDSPXMemAddress x = ((getpage() & 0xF) << 12) | (pz >> 4);
        const int v   = static_cast<int>(*x);

        const int off  = pz & 0xF;
        const int mask = 1 << off;
        const int res = (v & (~mask)) | ((bm & 0x1) << off);
        return static_cast<_fix_16>(res);
    }

    static _fix_16 PutNibble(const CDSPObject &bmp, const CDSPPZRegister &pzp)
    {
        const int pz  = static_cast<int>(pzp.GetValue());
        const int bm  = static_cast<int>(bmp.GetValue());
        CDSPXMemAddress x = ((getpage() & 0xC) << 12) | (pz >> 2);
        const int v   = static_cast<int>(*x);


        const int off  = 4 * (pz & 0x3);
        const int mask = 0xF << off;
        const int res = (v & (~mask)) | ((bm & 0xF) << off);
        return static_cast<_fix_16>(res);
    }

    static _fix_16 PutByte(const CDSPObject &bmp, const CDSPPZRegister &pzp)
    {
        const int pz  = static_cast<int>(pzp.GetValue());
        const int bm  = static_cast<int>(bmp.GetValue());
        CDSPXMemAddress x = ((getpage() & 0x8) << 12) | (pz >> 1);
        const int v   = static_cast<int>(*x);

        const int off  = 8 * (pz & 0x1);
        const int mask = 0xFF << off;
        const int res = (v & (~mask)) | ((bm & 0xFF) << off);
        return static_cast<_fix_16>(res);
    }

};

class CParallelizeHelper
{
public:
    CParallelizeHelper(void)
    {
        enterparallel();
    }
    CParallelizeHelper(bool)
    {
        enterparallel();
    }
    ~CParallelizeHelper(void)
    {
        leaveparallel();
    }
    operator bool(void)
    {
        return true;
    }
};

#define MAX_LOOP 20
class CDSPLoopManager
{
private:
    DWORD Address[MAX_LOOP + 1];
    int   MaxDepth;
public:
    CDSPLoopManager(void)
    {
        MaxDepth = 0;
        memset(Address, 0, sizeof(Address));
    }

    int AddAddress(const DWORD address)
    {
        int counter = 0;
        for (int i = 0; i <= MAX_LOOP; i++)
        {
            ++counter;
            if (0 == Address[i])
            {
                if (counter > MaxDepth)
                    MaxDepth = counter;
                else;
                Address[i] = address;
                return i;
            }
            else;
        }
        dspprint("EXCEPTION: CDSPLoopManager()'s run out of buffer, loop cannot be established!\n");
        return -1;
    }

    DWORD GetAddress(const int index)
    {
        if ((index >= 0) && (index <= MAX_LOOP))
            return Address[index];
        else
            return 0;       // must return 0 (0 is to identify a INVALID address)
    }

    int RemoveAddress(const int index)
    {
        if ((index >= 0) && (index <= MAX_LOOP))
        {
            Address[index] = 0;
            return index;
        }
        else
            return -1;
    }

    int GetMaxLoopDepth(void)
    {
        return MaxDepth;            
    }
};

class CDSPModule
{
protected:
    char m_szName[40];

public:
    CDSPModule(const char *name)
    {
        strcpy(m_szName, name);
    }

    virtual void Run(void)
    {
        try
        {
            AllocateLocalVariables();
            Initialization();
            Execute();
            Finalization();
            FreeLocalVariables();
        }        
        //catch (xception &e)
        //{
        //    dspprint("exception occured: %s\n", e.what());
        //}
        catch (...)
        {
            dspprint("unknown exception occured\n");
        }
    }

protected:
    virtual int  AllocateLocalVariables(void)
    {
        return 0;
    }

    virtual int  Initialization(void)
    {
        return 0;
    }
    
    virtual void Execute(void) = 0;
    
    virtual int  Finalization(void)
    {
        return 0;
    }

    virtual int  FreeLocalVariables(void)
    {
        return 0;
    }
};

const CDSPPointerRegister postinc(CDSPPointerRegister &reg, const CDSPObject &offset);
const CDSPPointerRegister postdec(CDSPPointerRegister &reg, const CDSPObject &offset);
const CDSPPointerRegister postinc(CDSPPointerRegister &reg, const _fix_40    offset);
const CDSPPointerRegister postdec(CDSPPointerRegister &reg, const _fix_40    offset);

CDSPMemoryObject & xmem(const int Address);
CDSPMemoryObject & ymem(const int Address);
CDSPMemoryObject & pmem(const int Address);

_fix_16 div(CDSP16BitRegister & r0, CDSP16BitRegister & x0, CDSP16BitRegister & dr);
_fix_16 norm(const CDSPObject & a0);
_fix_16 norm(const _fix_40 value);
_fix_16 pass(const CDSP16BitRegister & r0);
_fix_16 pass(const int value);
_fix_16 bitcount(const CDSP16BitRegister &reg);
_fix_16 max(const CDSP16BitRegister & a, const CDSP16BitRegister & b);
_fix_16 max(const _fix_16 a, const _fix_16 b);
_fix_40 lmax(const CDSPObject & a, const CDSPObject & b);
_fix_40 lmax(const _fix_40 a, const _fix_40 b);

_fix_16 getbit(const CDSPObject &bmp, const CDSPPZRegister &pzp);
_fix_16 getnibble(const CDSPObject &bmp, const CDSPPZRegister &pzp);
_fix_16 getcmi(const CDSPObject &bmp, const CDSPPZRegister &pzp);
_fix_16 getbyte(const CDSPObject &bmp, const CDSPPZRegister &pzp);
_fix_16 putbit(const CDSPObject &bmp, const CDSPPZRegister &pzp);
_fix_16 putnibble(const CDSPObject &bmp, const CDSPPZRegister &pzp);
_fix_16 putbyte(const CDSPObject &bmp, const CDSPPZRegister &pzp);
_fix_16 putn(const CDSPPZRegister &pzp);
_fix_16 expand(const CDSPObject &bm);
_fix_16 reverse(const CDSPObject &bm);

// Instruction-like functions
#define nop     0  
#define dbreak  lean.CustomPop()
#define lcc lcn.TopObj()
#define lea lean.TopObj()

#define repeat(n)   for (int i = 0; i < n; i++)
#define dcall(fun)  fun()

// ATTENTION: use __fastcall, then the first two DWORD parameters will be passed in ECX, EDX
int   __fastcall set_lsa(const DWORD address);
DWORD __fastcall get_lsa(void);

//#define _ENABLE_ASM_SUPPORT_FOR_LOOP
#ifdef  _ENABLE_ASM_SUPPORT_FOR_LOOP

#ifdef _MSC_VER
    #ifndef _M_IX86
        #error "x86 is required"
    #endif
#else
    #error "only support Microsoft C++ Compiler now"
#endif

// warning C4002: too many actual parameters for macro '%s'
#pragma warning(disable: 4002)

// ATTENTION: dangerous code, take care
// TRICKY   : here we use a call to get EIP which cannot be accessed directly
#define loop(count)                     \
{                                       \
    lean = random();                    \
    lcc  = count;                       \
    __asm { call $ + 5    } /* 5 */     \
    __asm { pop  eax      } /* 1 */     \
    __asm { mov  ecx, eax } /* 2 */     \
    __asm { add  eax, 0BH } /* 3 */     \
    __asm { push eax      } /* 1 */     \
    __asm { add  ecx, 10h } /* 3 */     \
    __asm { ret           } /* 1 */     \
    __asm { call set_lsa  } /* 5 */

/*
// ATTENTION: dangerous code, take care
#define loop(count, begin)              \
{                                       \
    lean = random();                    \
    __asm { mov  ecx, offset begin }    \
    __asm { call set_lsa           }    \
    lcc  = count;                       \
begin: 
                              
*/

#define endloop()                       \
    if (lcc.GetValue() != 1)            \
    {                                   \
        lcc = lcc - 1;                  \
        __asm {  call get_lsa  } /* 5 */\
        __asm {  or   eax, eax } /* 2 */\
        __asm {  jz   $ + 8    } /* 6 */\
        __asm {  jmp  eax      } /* 2 */\
    }                                   \
    else                                \
        dbreak;                         \
}

#else 
        
#define loop(count, begin)              \
{                                       \
    lean = random();                    \
    lcc  = count;                       \
    lsa  = random();                    \
begin:                                  

#define endloop(begin)                  \
    if (lcc.GetValue() != 1)            \
    {                                   \
        lcc = lcc - 1;                  \
        goto begin;                     \
    }                                   \
    else                                \
        dbreak;                         \
}

#endif

#define gotonear goto
#define gotofar  goto

#define para     if (CParallelizeHelper ParallelizeHelper = true) 
#define asi      para

int prealloc(const int Size, 
             const CMemClass MemClass = XMem, 
             const CMemoryAllocationScheme Scheme = DEFAULT_SCHEME, 
             const int Address = 0);
int dalloc(const int Size, 
           const CMemClass MemClass = XMem, 
           const CMemoryAllocationScheme Scheme = DEFAULT_SCHEME, 
           const int Address = 0);
int  dfree(const int Address, const CMemClass MemClass = XMem, const int Size = 0);
int  dlen(const int Address, const CMemClass MemClass = XMem);

#endif