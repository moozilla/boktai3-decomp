#include "global.h"

extern u8 *gUnk_0200004C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneIndicators_Initialize(u8 *);
void Task_Kill(u8 *);
void SceneIndicators_Update(void);
void sub_08011AA4(void);

u8 *sub_08011C00(void)
{
    u8 *p;
    if (gUnk_0200004C != 0)
        return gUnk_0200004C;
    p = Task_Create(0x8, 0xbd0);
    if (p != 0) {
        Task_SetCallbacks(p, SceneIndicators_Update, sub_08011AA4);
        if (SceneIndicators_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
