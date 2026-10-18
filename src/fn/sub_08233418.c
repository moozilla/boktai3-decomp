#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void ExplosionEffect_Update(u8 *);
void ExplosionEffect_Destroy(u8 *);
void Task_Kill(u8 *);
s32 ExplosionEffect_Init(u8 *, u8 *, u32, s32, s32, s32, s32, s32, u32);
u8 *ExplosionEffect_CreateDelayed(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 frame, u32 arg5)
{
    u8 *obj = Task_Create(10, 184);
    if (obj) {
        Task_SetCallbacks(obj, ExplosionEffect_Update, ExplosionEffect_Destroy);
        if (ExplosionEffect_Init(obj, pos, flag, duration, arg3, 0x240004, 60, frame, arg5) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
