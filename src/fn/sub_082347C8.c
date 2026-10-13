#include "global.h"
struct Pair47 {
    u32 a, b;
};
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void Task_Kill(u8 *);
void sub_0823450C(u8 *);
void sub_08234520(u8 *);
s32 sub_08234770(u8 *, u8 *, struct Pair47 *);
u8 *sub_082347C8(u8 *p, struct Pair47 *q)
{
    u8 *obj = Task_Create(8, 0x208);
    if (obj) {
        Task_SetCallbacks(obj, sub_0823450C, sub_08234520);
        if (sub_08234770(obj, p, q) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
