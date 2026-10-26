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
extern void *gUnk_086056D8[];
void Callback_InvokeActor(struct Transition *, void *);
int DialogueTransition_OnTaskUpdate(struct Transition *p)
{
    void *callback = gUnk_086056D8[p->state];
    if (callback)
        Callback_InvokeActor(p, callback);
    {
        u16 *reg = &gUnk_03004FC0;
        int value = 0x1084;
        *reg = value;
    }
    if ((gUnk_03005260[1] & 9) && p->endProcedure && p->state <= 2)
        DialogueTransition_SetState(p, 3);
    p->ticks++;
    return 0;
}
