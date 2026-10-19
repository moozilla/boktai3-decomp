#define SceneActorSequence_UpdateFadeInHold sub_081C07C8
#include "global.h"

struct ScreenTransition {
    u8 unknown0[0x20];
    s16 x;
    u8 unknown22[6];
    s16 y;
    u8 unknown2A[6];
    void *child;
    u8 unknown30[0x20];
    s32 timer;
};

u32 RegistryF3E4_GetStateResult(u8 *p);
void SceneActorSequence_BeginCenterMotion(struct ScreenTransition *p);
s32 __divsi3(s32, s32);

void SceneActorSequence_UpdateFadeInHold(struct ScreenTransition *p)
{
    s32 v;

    {
        s32 duration = 0x40;
        s32 timer = p->timer;
        if (timer <= 0) {
            v = 192;
        } else if (timer >= duration) {
            v = 168;
        } else {
            s32 k = -6144;
            k *= timer;
            k = __divsi3(k, duration);
            if (k >= 0)
                k >>= 8;
            else
                k = -((-k) >> 8);
            v = k + 192;
        }
        p->x = v;
    }

    {
        s32 duration = 0x40;
        s32 timer = p->timer;
        if (timer <= 0) {
            v = 32;
        } else if (timer >= duration) {
            v = 16;
        } else {
            s32 k = -4096;
            k *= timer;
            k = __divsi3(k, duration);
            if (k >= 0)
                k >>= 8;
            else
                k = -((-k) >> 8);
            v = k + 32;
        }
        p->y = v;
    }

    {
        s32 n = p->timer + 1;
        p->timer = n;
    }

    if (RegistryF3E4_GetStateResult(p->child)) {
        if (p->timer > 63)
            SceneActorSequence_BeginCenterMotion(p);
    }
}
