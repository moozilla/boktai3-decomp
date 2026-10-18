#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void Explosion_Update(u8 *);
void Explosion_Destroy(u8 *);
void Task_Kill(u8 *);
s32 Explosion_Init(u8 *, u8 *, u32, s32, s32, s32, s32, s32, u32);
u8 *Explosion_Create(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, s32 arg5, s32 arg6,
                     u32 arg7)
{
    u8 *obj = Task_Create(10, 188);
    if (obj) {
        Task_SetCallbacks(obj, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(obj, pos, flag, duration, arg3, arg4, arg5, arg6, arg7) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
