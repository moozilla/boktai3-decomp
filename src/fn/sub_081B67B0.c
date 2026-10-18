#include "global.h"
struct Pair3B {
    u32 a, b;
};
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void Task_Kill(u8 *);
void sub_081B6690(u8 *);
void sub_081B66C8(u8 *);
s32 sub_081B66E0(u8 *, struct Pair3B *, u32, u32, u32, u32);
u8 *sub_081B67B0(struct Pair3B *p, u32 a, u32 b, u32 c, u32 d)
{
    u8 *obj = Task_Create(8, 0xA0);
    if (obj) {
        Task_SetCallbacks(obj, sub_081B6690, sub_081B66C8);
        if (sub_081B66E0(obj, p, a, b, c, d) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
