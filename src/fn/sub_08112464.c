#define MessagePanel_UpdateExitDelay     sub_08112464
#define MessagePanel_UpdateBlink StageResults_UpdateRecordBlink
#define MessagePanel_FinishExit StageResults_FinishDisplay
#define MessagePanel_StepPairedAnimation CharacterModel_UpdatePairedAnimations
#define MessagePanel_RotatePalette PaletteFade_CycleMenuColors
#include "global.h"
struct MessagePanel {
    u8 pad0[0x11B8];
    void (*callback)(void *);
    u8 pad11BC[0xC];
    u8 state;
    u8 pad11C9;
    u8 entering;
    u8 pad11CB[0x3F];
    u16 exitTimer;
};
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void CharacterModel_UpdatePairedAnimations(void *, s32);
void PaletteFade_CycleMenuColors(void *);
void StageResults_UpdateRecordBlink(void *);
void StageResults_FinishDisplay(void *);
void ScriptDisplay_OnTransitionEnd(void *);
static inline void SetState(u8 *p, void (*cb)(void *), u8 state)
{
    *(void (**)(void *))(p + 0x11B8) = cb;
    p[0x11CA] = 1;
    p[0x11C8] = state;
}
void sub_08112464(struct MessagePanel *p)
{
    if (p->entering) {
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xFFFF, 0);
        p->exitTimer = 0;
        p->entering = 0;
    }
    CharacterModel_UpdatePairedAnimations((u8 *)p + 0xF60, 0);
    PaletteFade_CycleMenuColors((u8 *)p + 0x944);
    StageResults_UpdateRecordBlink(p);
    if (p->exitTimer <= 31) {
        p->exitTimer++;
    } else {
        StageResults_FinishDisplay(p);
        SetState((u8 *)p, ScriptDisplay_OnTransitionEnd, 2);
    }
}
