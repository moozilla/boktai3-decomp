#include "global.h"
void DarkLoan_StartDisplayScript(void);
void sub_081BCA74(u8 *);
void sub_081BC6AC(u8 *);
void DarkLoan_OpenDisplay(u8 *p)
{
    DarkLoan_StartDisplayScript();
    if (*(u16 *)(p + 0x906))
        sub_081BCA74(p);
    else
        sub_081BC6AC(p);
}
