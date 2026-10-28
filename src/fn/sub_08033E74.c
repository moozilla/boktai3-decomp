#include "global.h"
#include "render.h"
#include "ram.h"
#include "input.h"
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
int DialogueTransition_OnTaskDestroy(struct Transition *p)
{
    gTransition = 0;
    MessageWindow_Clear();
    if (p->flags & 1)
        gDispCntShadow = p->savedRegister;
    return 0;
}
