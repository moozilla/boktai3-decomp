#include "global.h"
struct Pair3B {
    u32 a, b;
};
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void Task_Kill(u8 *);
void ActionFlash_Update(u8 *);
void ActionFlash_Destroy(u8 *);
s32 ActionFlash_Initialize(u8 *, struct Pair3B *, u32, u32, u32, u32);
u8 *ActionFlashBurst_Create(struct Pair3B *p, u32 a, u32 b, u32 c, u32 d)
{
    u8 *obj = Task_Create(8, 0x470);
    if (obj) {
        Task_SetCallbacks(obj, ActionFlash_Update, ActionFlash_Destroy);
        if (ActionFlash_Initialize(obj, p, a, b, c, d) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
