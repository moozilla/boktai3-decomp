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
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 Script_GetTextSourcePc(void);
s32 DarkLoan_GetDefaultRate(void);
void DarkLoan_StartDisplayScript(struct ScriptPanel *);
void ScriptPanel_InitializeProcedure(struct ScriptPanel *);
void DarkLoan_BuildDisplay(struct ScriptPanel *, s32);
void DarkLoan_BeginAlternateOpening(struct ScriptPanel *);
void DarkLoan_BeginOpening(struct ScriptPanel *);

s32 DarkLoan_InitializeFromScript(struct ScriptPanel *panel)
{
    u32 callbackPc;
    if (Script_SeekToKeyword(0x52)) {
        panel->rate = Script_GetValue();
        if (panel->rate <= 1)
            panel->rate = 2;
        if (panel->rate > 9)
            panel->rate = 9;
    } else {
        panel->rate = DarkLoan_GetDefaultRate();
    }
    if (Script_SeekToKeyword(0x64))
        panel->valueD = Script_GetValue();
    else
        panel->valueD = 0;
    if (Script_SeekToKeyword(0x65))
        panel->valueE = Script_GetValue();
    else
        panel->valueE = 0;
    if (Script_SeekToKeyword(0x6F))
        panel->displayMode = Script_GetValue();
    else
        panel->displayMode = 0;
    if (Script_SeekToKeyword(0x72))
        callbackPc = Script_GetTextSourcePc();
    else
        callbackPc = 0;
    DarkLoan_StartDisplayScript(panel);
    ScriptPanel_InitializeProcedure(panel);
    DarkLoan_BuildDisplay(panel, 0);
    panel->callbackPc = callbackPc;
    if (panel->displayMode)
        DarkLoan_BeginAlternateOpening(panel);
    else
        DarkLoan_BeginOpening(panel);
    return 0;
}
