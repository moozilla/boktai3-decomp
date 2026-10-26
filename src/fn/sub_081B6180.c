#include "global.h"
#include "ram.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void Task_Kill(void *);
void PairedSceneActor_UpdateHalves(void);
void sub_081B5E38(void);
s32 sub_081B5EA4(void *, s32, s32, s32);
void *Vanargand_CreatePartController(s32 a, s32 b, s32 c)
{
    void **global = &gVanargandPartController;
    void *p = Task_Create(9, 0x5b8);
    if (p) {
        Task_SetCallbacks(p, PairedSceneActor_UpdateHalves, sub_081B5E38);
        *global = p;
        if (sub_081B5EA4(p, a, b, c) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
