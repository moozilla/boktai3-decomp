#include "dark_loan_screen.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void DarkLoan_OnUpdate(void);
void DarkLoan_OnDestroy(void);
s32 DarkLoan_GetDefaultRate(void);
void ScriptPanel_InitializeProcedure(struct DarkLoanScreen *);
void DarkLoan_HideDisplay(void *);

struct DarkLoanScreen *DarkLoan_CreateHiddenDisplay(u32 rate, s32 valueD, u32 displayMode,
                                                 u32 callbackPc)
{
    struct DarkLoanScreen *panel;
    u16 *rateField;
    panel = Task_Create(0xB, sizeof(struct DarkLoanScreen));
    if (panel)
        Task_SetCallbacks(panel, DarkLoan_OnUpdate, DarkLoan_OnDestroy);
    do {
        rateField = &panel->rate;
        *rateField = rate;
        if (rate - 2 > 7)
            *rateField = DarkLoan_GetDefaultRate();
    } while (0);
    ScriptPanel_InitializeProcedure(panel);
    DarkLoan_BuildDisplay(panel, 1);
    panel->callbackPc = callbackPc;
    panel->valueD = valueD;
    panel->valueE = 0;
    panel->displayMode = displayMode;
    DarkLoan_HideDisplay(panel);
    return panel;
}
