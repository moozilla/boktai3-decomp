#include "global.h"

struct ScriptPanel {
    u8 pad0[0x8E4];
    void *procedureData;
    u32 callbackPc;
    u16 rate;
    u8 pad8EE[2];
    s32 valueD;
    s32 valueE;
    u8 pad8F8[0xE];
    u16 displayMode;
    u8 pad908[7];
    u8 state;
    u32 snapshot[8];
};
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void DarkLoan_OnUpdate(void);
void DarkLoan_OnDestroy(void);
s32 DarkLoan_GetDefaultRate(void);
void ScriptPanel_InitializeProcedure(struct ScriptPanel *);
void DarkLoan_BuildDisplay(struct ScriptPanel *, s32);
void DarkLoan_HideDisplay(void *);

struct ScriptPanel *DarkLoan_CreateHiddenDisplay(u32 rate, s32 valueD, u32 displayMode,
                                                 u32 callbackPc)
{
    int new_var;
    u8 *panel;
    u16 *rateField;
    new_var = 0xB;
    panel = Task_Create(new_var, 0x934);
    if (panel)
        Task_SetCallbacks(panel, DarkLoan_OnUpdate, DarkLoan_OnDestroy);
    do {
        rateField = (u16 *)(panel + 0x8EC);
        *rateField = rate;
        if (rate - 2 > 7)
            *rateField = DarkLoan_GetDefaultRate();
    } while (0);
    ScriptPanel_InitializeProcedure((struct ScriptPanel *)panel);
    DarkLoan_BuildDisplay((struct ScriptPanel *)panel, 1);
    ((struct ScriptPanel *)panel)->callbackPc = callbackPc;
    ((struct ScriptPanel *)panel)->valueD = valueD;
    ((struct ScriptPanel *)panel)->valueE = 0;
    ((struct ScriptPanel *)panel)->displayMode = displayMode;
    DarkLoan_HideDisplay(panel);
    return (struct ScriptPanel *)panel;
}
