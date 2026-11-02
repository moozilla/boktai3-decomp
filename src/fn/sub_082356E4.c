#include "health_burst.h"
#include "render.h"
#include "engine_globals.h"

void Sound_PlaySong(u32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 Collider_Refresh(void *);
void Task_Kill(void *);
void Camera_RequestShake(s32);

static inline u32 Mask(u32 *a, u32 m)
{
    return *a & m;
}

static inline u16 HealthBurst_WrappedFrame(u32 next)
{
    u32 frame = next & 0xffff;
    return frame;
}

void sub_082356E4(struct HealthBurstTask *p)
{
    u16 *counter = &p->frame;
    u16 *updatedTimer;
    s32 nextFrame;

    if (*counter == 0) {
        gEnginePauseFlags = Mask(&gEnginePauseFlags, ~4);
        Sound_PlaySong(0x67);
        ScreenTransition_SendCommand(2, 4, 0x1f, 0x1f, 0x1f, 0x1fff, 1);
    }
    if (*counter <= 5) {
        Collider_Refresh(p->collider);
    } else if (*counter == 6) {
        gEngineFlags &= ~1;
    }

    updatedTimer = &p->frame;
    nextFrame = *updatedTimer + 1;
    *updatedTimer = nextFrame;
    if (HealthBurst_WrappedFrame(nextFrame) > 47) {
        Task_Kill(p);
        return;
    }
    if ((u16)nextFrame <= 15)
        Camera_RequestShake(32);
    else
        Camera_RequestShake(48 - *updatedTimer);
}
