#include "dark_loan_screen.h"

s32 ScriptPanel_IsCallbackActive(struct DarkLoanScreen *panel)
{
    if (panel->callback == DarkLoan_UpdateHidden)
        return 1;
    return 0;
}
