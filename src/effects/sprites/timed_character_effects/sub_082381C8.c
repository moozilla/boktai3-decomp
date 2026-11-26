#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void PlayerOrbitEffect_UpdateMovingParticle(u8 *);
void TimedSpriteEffect_Destroy(u8 *);
void Task_Kill(u8 *);
s32 PlayerOrbitEffect_InitMovingParticle(u8 *, u8 *, u32, s32, s32, s32, s32, s32, s32, u32);
u8 *TimedSpriteEffect_Create(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, s32 arg5,
                             s32 arg6, s32 arg7, u32 arg8)
{
    u8 *obj = Task_Create(8, 196);
    if (obj) {
        Task_SetCallbacks(obj, PlayerOrbitEffect_UpdateMovingParticle, TimedSpriteEffect_Destroy);
        if (PlayerOrbitEffect_InitMovingParticle(obj, pos, flag, duration, arg3, arg4, arg5, arg6,
                                                 arg7, arg8) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
