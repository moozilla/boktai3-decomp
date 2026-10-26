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
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int DialogueTransition_LoadFromScript(struct Transition *);
int sub_08033FEC(struct Transition *);
int DialogueTransition_OnTaskUpdate(struct Transition *);
int DialogueTransition_OnTaskDestroy(struct Transition *);
struct Transition *DialogueTransition_CreateFromScript(void)
{
    struct Transition *p;
    if (gUnk_020000E8) {
        DialogueTransition_LoadFromScript(gUnk_020000E8);
        return gUnk_020000E8;
    }
    p = Task_Create(8, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, DialogueTransition_OnTaskUpdate, DialogueTransition_OnTaskDestroy);
        if (sub_08033FEC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
