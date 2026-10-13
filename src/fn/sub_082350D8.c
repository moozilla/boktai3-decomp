#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void Task_Kill(u8 *);
void sub_08234ECC(u8 *);
void sub_08234EF4(u8 *);
s32 sub_08235000(u8 *, u8 *);
u8 *sub_082350D8(u8 *p)
{
    u8 *obj = Task_Create(11, 0x278);
    if (obj) {
        Task_SetCallbacks(obj, sub_08234ECC, sub_08234EF4);
        if (sub_08235000(obj, p) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
