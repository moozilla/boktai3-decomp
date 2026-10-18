#include "global.h"
struct Pair47 {
    u32 a, b;
};
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void Task_Kill(u8 *);
void SpiralBurst_OnTaskUpdate(u8 *);
void SpiralBurst_OnTaskDestroy(u8 *);
s32 SpiralBurst_Init(u8 *, u8 *, struct Pair47 *);
u8 *SpiralBurst_Create(u8 *p, struct Pair47 *q)
{
    u8 *obj = Task_Create(8, 0x208);
    if (obj) {
        Task_SetCallbacks(obj, SpiralBurst_OnTaskUpdate, SpiralBurst_OnTaskDestroy);
        if (SpiralBurst_Init(obj, p, q) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
