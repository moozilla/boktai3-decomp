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
extern u16 gUnk_03005260[];
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
void DialogueTransition_UpdateFadeOut(struct Transition *p)
{
    DialogueTransition_ConsumeStateEntry(p);
    if (p->ticks >= p->fadeOut[p->index]) {
        gBgFadeLevel = 0;
        if (p->index == p->lastIndex) {
            MessageWindow_Clear();
            DialogueTransition_SetState(p, 4);
            if (p->startProcedure)
                Script_Call(p->startProcedure, 0);
            if (p->flags & 2)
                Task_Kill(p);
        } else {
            p->index++;
            DialogueTransition_SetState(p, 0);
        }
    } else
        gBgFadeLevel = 64 - Div(p->ticks << 6, p->fadeOut[p->index]);
}
