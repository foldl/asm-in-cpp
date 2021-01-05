#include "DSP_CPP.h"
#include "DSPIO.h"
#include "DSPCore.h"

class CDSPEvaluator;
CDSPEvaluator *pCurEval = NULL;
void evalset(CDSPEvaluator *Eval);
void evalpush(CDSPObject *Operand);

#define MAX_OPERAND_COUNT 3
class CDSPEvaluator
{
private:
    CDSPObject *Operands[MAX_OPERAND_COUNT];
    int OperandNext;
    int OperandsNeeded;
    char Name[MAX_NAME + 1];

public:
    CDSPEvaluator(const int OperandsNeeded, const char *Name)
    {
        this->OperandsNeeded = OperandsNeeded;
        ClearOperands();
        this->Name[0] = '\0';
        if ((Name != NULL) && (strlen(Name) <= MAX_NAME))
            strcpy(this->Name, Name);
    }

    CDSPObject *GetOperand(const int Index)
    {
        if ((Index >= 0) && (Index < MAX_OPERAND_COUNT))
            return Operands[Index];
        else
            return NULL;
    }

    void ClearOperands(void)
    {
        memset(Operands, 0, sizeof(Operands));
        OperandNext = 0;
    }

    bool PushOperand(CDSPObject *Operand)
    {
        if ((OperandNext >= 0) && (OperandNext < MAX_OPERAND_COUNT) && (NULL != Operand))
            Operands[OperandNext++] = Operand;
        if (OperandNext >= OperandsNeeded)
        {
            Execute();
            ClearOperands();
            evalset(NULL);
        }

        return true;
    }

    const char *GetName(void)
    {
        return Name;
    }
protected:
    virtual void Execute(void) = 0;
};

class CDSPEvalMov : public CDSPEvaluator
{
public:
    CDSPEvalMov(void)
        : CDSPEvaluator(2, "mov")
    {
    }
protected:
    virtual void Execute(void)
    {
        *GetOperand(0) = *GetOperand(1);
    }
};

class CDSPEvalAdd : public CDSPEvaluator
{
public:
    CDSPEvalAdd(void)
        : CDSPEvaluator(2, "add")
    {
    }
protected:
    virtual void Execute(void)
    {
        *GetOperand(0) = *GetOperand(0) + *GetOperand(1);
    }
};

class CDSPEvalXor : public CDSPEvaluator
{
public:
    CDSPEvalXor(void)
        : CDSPEvaluator(2, "xor")
    {
    }
protected:
    virtual void Execute(void)
    {
        *GetOperand(0) = *GetOperand(0) ^ *GetOperand(1);
    }
};

void evalset(CDSPEvaluator *Eval)
{
    if ((NULL != pCurEval) && (NULL != Eval))
        dspprint("FATAL: '%s' operation is waiting for more input\n", pCurEval->GetName());
    pCurEval = Eval;
}

void evalpush(CDSPObject *Operand)
{
    if (NULL != pCurEval)
        pCurEval->PushOperand(Operand);
    else
        dspprint("FATAL: instruction missing\n");
}

CDSPEvalMov EvalMov;
CDSPEvalAdd EvalAdd;
CDSPEvalXor EvalXor;

#define MOV evalset(&EvalMov),
#define ADD evalset(&EvalAdd),
#define XOR evalset(&EvalXor),

#define R0 evalpush(&r0)
#define R1 evalpush(&r1)
#define R2 evalpush(&r2)
#define R3 evalpush(&r3)
#define R4 evalpush(&r4)
#define R5 evalpush(&r5)
#define R6 evalpush(&r6)
#define R7 evalpush(&r7)