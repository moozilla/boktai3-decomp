#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 FragmentBurst_InitPool(u8 *);
void Task_Kill(u8 *);
void sub_08128A00(void);
void SpriteGroupPool_Destroy(void);

u8 *FragmentBurst_EnsurePool(void)
{
    u8 *p;
    if (gSpriteGroupPool != 0)
        return gSpriteGroupPool;
    p = Task_Create(0xa, 0x5bc);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08128A00, SpriteGroupPool_Destroy);
        if (FragmentBurst_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
