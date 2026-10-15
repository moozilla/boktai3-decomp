#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ResourceStation_Initialize(u8 *, u32, u32);
void Task_Kill(u8 *);
void ResourceStation_Tick(void);
void ResourceStation_Destroy(void);
u8 *ResourceStation_CreateFromScript(u32 a, u32 b)
{
    u8 *r = Task_Create(8, 0x268);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void))ResourceStation_Tick,
                          (void (*)(void))ResourceStation_Destroy);
        if (ResourceStation_Initialize(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
