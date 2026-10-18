#include "global.h"

void *ResourceRegistry_GetResource(u32);
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081DA684(u8 *, s32);
void Task_Kill(u8 *);
void sub_081DA3D8(void);
void sub_081DA61C(void);
/* The task tagged 0x4E01 (ResourceRegistry_GetResource), or a new one (0x10C bytes) set up
   by sub_081DA684; NULL when that fails (tentative). */
u8 *Task4E01_GetOrCreate(void)
{
    u8 *p = ResourceRegistry_GetResource(0x4E01);
    if (p)
        return p;
    p = Task_Create(8, 0x10C);
    if (p) {
        Task_SetCallbacks(p, sub_081DA3D8, sub_081DA61C);
        if (sub_081DA684(p, 12) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
