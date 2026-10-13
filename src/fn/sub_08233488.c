#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void sub_082331E4(u8 *);
void sub_08233248(u8 *);
void Task_Kill(u8 *);
s32 sub_0823333C(u8 *, u8 *, u32, s32, s32, s32, s32, s32, u32);
u8 *sub_08233488(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, s32 arg5, s32 arg6, u32 arg7)
{
    u8 *obj = Task_Create(10, 184);
    if (obj) {
        Task_SetCallbacks(obj, sub_082331E4, sub_08233248);
        if (sub_0823333C(obj, pos, flag, duration, arg3, arg4, arg5, arg6, arg7) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
