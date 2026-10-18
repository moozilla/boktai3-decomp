#include "global.h"
void DarkLoan_HideSprites(void);
void DarkLoan_SetState(void *, void (*)(void));
void sub_081BCB14(void);
void DarkLoan_HideDisplay(void *p)
{
    DarkLoan_HideSprites();
    DarkLoan_SetState(p, sub_081BCB14);
}
