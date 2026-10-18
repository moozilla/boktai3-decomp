#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_081D7300(void *, void *);
void Task_Kill(void *);
void ScreenTransition_DispatchPhase(void);
void ScreenTransition_ReturnZero(void);
void *TextSequence_CreateFromScript(void *a)
{
    void *p = Task_Create(8, 0x328);
    if (p) {
        Task_SetCallbacks(p, ScreenTransition_DispatchPhase, ScreenTransition_ReturnZero);
        if (sub_081D7300(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
