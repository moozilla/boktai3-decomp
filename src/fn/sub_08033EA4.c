// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
struct Transition {
    u8 pad0[0x18];
    u8 state, entered, index, lastIndex;
    u32 parameter, ticks;
    u16 flags, savedRegister;
    void *script;
    void *entries;
    u16 startProcedure, endProcedure;
    s16 delays[64];
    u16 fadeIn[64], fadeOut[64], x[64], y[64], width[64], height[64];
};
extern struct Transition *gUnk_020000E8;
extern u16 gUnk_03004FC0, gUnk_03005260[];
void MessageWindow_SetBounds(int, int, int, int);
void MessageWindow_ShowMessage(int);
void MessageWindow_Clear(void);
void MessageWindow_SetDisplayMode(int);
void MessageWindow_SetMessageTable(void *);
void MessageWindow_SetPalette(int);
void Script_Call(int, int);
void Task_Kill(void *);
void Bg_ClearTilemap(int);
int Div(int, int);
u8 DialogueTransition_ConsumeStateEntry(struct Transition *);
void DialogueTransition_SetState(struct Transition *, int);
void DialogueTransition_ApplyEntryBounds(struct Transition *);
int Script_SeekToKeyword(int);
void *Script_GetTextSourcePc(void);
void *sub_08227E9C(void);
void Script_SetPc(void *);
int Script_GetValue(void);
int Script_GetKeywordValue(int, int);
int DialogueTransition_LoadFromScript(struct Transition *p)
{
    int zero = 0;
    p->state = zero;
    p->entered = zero;
    p->ticks = zero;
    p->index = Script_GetKeywordValue(112, 0);
    p->parameter = zero;
    if (!Script_SeekToKeyword(115))
        goto fail;
    p->script = Script_GetTextSourcePc();
    if (p->script)
        goto parameters;
fail:
    return -1;
parameters:
    if (Script_SeekToKeyword(116)) {
        int i;
        int sentinel;
        s16 *delay;
        u16 *fadeIn;
        p->entries = sub_08227E9C();
        Script_SetPc(p->entries);
        i = 0;
        sentinel = -1;
        delay = p->delays;
        fadeIn = p->fadeIn;
        do {
            int offset = i;
            int value = Script_GetValue();
            asm("" : : "r"(offset << 1));
            *delay = value;
            p->x[offset] = Script_GetValue();
            p->y[offset] = Script_GetValue();
            p->width[offset] = Script_GetValue();
            p->height[offset] = Script_GetValue();
            *fadeIn = Script_GetValue();
            p->fadeOut[offset] = Script_GetValue();
            if (i >= p->index && *delay == sentinel) {
                p->lastIndex = i - 1;
                goto ready;
            }
            delay++;
            fadeIn++;
            i++;
        } while (i <= 63);
    }
ready: {
    int procedure = Script_GetKeywordValue(101, 0);
    zero = 0;
    p->startProcedure = procedure;
}
    p->endProcedure = Script_GetKeywordValue(99, 0);
    MessageWindow_SetMessageTable(p->script);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_SetPalette(0x539C);
    MessageWindow_ShowMessage(p->index);
    DialogueTransition_ApplyEntryBounds(p);
    gBgFadeColor = 0x1084;
    gBgFadeBankMask = 0x8000;
    gBgFadeLevel = zero;
    DialogueTransition_SetState(p, 0);
    return 0;
}
