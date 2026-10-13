#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void sub_081DAF50(u8 *);
void sub_081DAFCC(u8 *);
void Task_Kill(u8 *);
s32 sub_081DB168(u8 *, u8 *, u32, s32, s32, s32, s32, s32, u32);
u8 *sub_081DB2B8(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, u32 arg5)
{
    u8 *obj = Task_Create(10, 188);
    if (obj) {
        Task_SetCallbacks(obj, sub_081DAF50, sub_081DAFCC);
        if (sub_081DB168(obj, pos, flag, duration, arg3, 262144, 60, arg4, arg5) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
