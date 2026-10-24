#include "global.h"

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
    void (*nextCallback)(void *);
    u8 *entering;
    u8 *state;
    struct MessagePanel *owner;
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
    // Recover the panel that owns the callback slot.
    owner = (struct MessagePanel *)((u8 *)callback - (u32) & ((struct MessagePanel *)0)->callback);
    entering = &owner->entering;
    state = &p->state;
    if (found) {
        s32 scriptPc = Script_GetValueSafe();
        if (!scriptPc) {
            s32 *q;
            s32 initial = 6000;
            i = 11;
            q = &p->parameters[11];
            do {
                *(q--) = initial;
            } while ((--i) >= 0);
        } else {
            Script_SetPc(scriptPc);
            i = 0;
            goto read_parameter;
        store_parameter:
            p->parameters[i] = value;
            i++;
        read_parameter:
            if (i <= 10) {
                value = Script_GetValue();
                if (value >= 0)
                    goto store_parameter;
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
    nextCallback = sub_08112508;
    zero = 0;
    *callback = nextCallback;
    *entering = 1;
    *state = zero;
    if (Script_GetKeywordValue(0x66, 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, zero);
    return 1;
}
