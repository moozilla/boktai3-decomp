#include "global.h"
void DarkLoan_StartDisplayScript(void);
void DarkLoan_BeginAlternateOpening(u8 *);
void DarkLoan_BeginOpening(u8 *);
void DarkLoan_OpenDisplay(u8 *p)
{
    DarkLoan_StartDisplayScript();
    if (*(u16 *)(p + 0x906))
        DarkLoan_BeginAlternateOpening(p);
    else
        DarkLoan_BeginOpening(p);
}
