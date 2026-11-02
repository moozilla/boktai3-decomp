#include "dark_loan_screen.h"

void sub_081BCB14(void);

s32 ScriptPanel_IsCallbackActive(struct DarkLoanScreen *panel)
{
    if (panel->callback == sub_081BCB14)
        return 1;
    return 0;
}
