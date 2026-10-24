#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// foundry draft (worker best 80.4%); what differs is mostly register allocation and instruction order
struct MessagePanel {
    u8 pad0[0x20];
    void *palette;
    u8 pad24[0x1194];
    void (*callback)(void *);
    u32 resource;
    u32 script;
    u8 pad11C4[4];
    u8 state;
    u8 pad11C9;
    u8 entering;
    u8 pad11CB[0xD];
    s32 parameters[12];
    u8 pad1208[8];
    s32 exitScript;
};
s32 Script_SeekToKeyword(u32);
s32 Script_GetPc(void);
s32 Script_GetValueSafe(void);
s32 Script_GetValue(void);
void Script_SetPc(s32);
s32 Script_GetKeywordValue(u32, s32);
u32 Script_GetTextSourcePc(void);
void Bg_ClearTilemap(s32);
void StageResults_LoadBackgroundResources(void *);
void StageResults_InitAnimationIds(void *);
void MessagePanel_InitSprites(void *);
void StageResults_InitPortraitDisplay(void *);
void StageResults_InitCursorPositions(void *);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
void StageResults_CreateTextWindows(void *);
void StageResults_ShowSummaryText(void *);
void sub_08112508(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 MessagePanel_ConfigureFromScript(struct MessagePanel *p)
{
    s32 i;
    s32 value;
    s32 found;
    s32 zero;
    void (**callback)(void *);
    u8 *entering;
    u8 *state;
    if (Script_SeekToKeyword(0x6E)) {
        p->script = Script_GetPc();
        if (!p->script)
            return -1;
    }
    if (Script_SeekToKeyword(0x72)) {
        p->resource = Script_GetTextSourcePc();
        if (!p->resource)
            return -1;
    }
    found = Script_SeekToKeyword(0x62);
    callback = &p->callback;
    entering = &p->entering;
    state = (u8 *)(((u32)p) + 0x11C8);
    if (found) {
        value = Script_GetValueSafe();
        if (!value) {
            s32 *q;
            s32 initial = 6000;
            i = 11;
            q = &p->parameters[11];
            do {
                *(q--) = initial;
            } while ((--i) >= 0);
        } else {
            Script_SetPc(value);
            for (i = 0; i <= 10; i++) {
                value = Script_GetValue();
                if (value < 0)
                    break;
                *((s32 *)((((u32)p) + 0x11D8) + (i << 2))) = value;
            }
        }
    }
    p->exitScript = Script_GetKeywordValue(0x65, 0);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    StageResults_LoadBackgroundResources(p);
    StageResults_InitAnimationIds(p);
    MessagePanel_InitSprites(p);
    StageResults_InitPortraitDisplay(p);
    StageResults_InitCursorPositions(p);
    ModifierMenu_InitSelectionSnapshot(((u8 *)p) + 0x944, p->palette);
    StageResults_CreateTextWindows(p);
    StageResults_ShowSummaryText(p);
    zero = 0;
    *(&p->callback) = sub_08112508;
    *entering = 1;
    *state = zero;
    if (Script_GetKeywordValue(0x66, 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, zero);
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", MessagePanel_ConfigureFromScript);
#endif
