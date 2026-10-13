#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C1578(void *);
void Task_Kill(void *);
u32 sub_081C1384(void *);
u32 sub_081C13C0(void *);
void *sub_081C1788(void)
{
    void *p = Task_Create(0xb, 0x464);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))sub_081C1384, (void (*)(void *))sub_081C13C0);
        if (sub_081C1578(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
