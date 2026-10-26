#include "global.h"
#include "render.h"
#include "ram.h"
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
void DialogueTransition_UpdateSkipFadeOut(struct Transition *p)
{
    DialogueTransition_ConsumeStateEntry(p);
    if (p->endProcedure) {
        if (p->ticks > 15) {
            u32 *brightness = &gBgFadeLevel;
            int zero = 0;
            *brightness = zero;
            Script_Call(p->endProcedure, 0);
            p->endProcedure = zero;
            Task_Kill(p);
        } else
            gBgFadeLevel = 64 - Div(p->ticks << 6, 16);
    }
}
