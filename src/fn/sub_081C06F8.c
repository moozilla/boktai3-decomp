#define SceneActorSequence_UpdateFadeIn sub_081C06F8
#include "global.h"

struct ScreenTransition {
    u8 unknown0[0x20];
    s16 x;
    u8 unknown22[6];
    s16 y;
    u8 unknown2A[0x2A];
    s32 timer;
};

void ScreenTransition_BeginDownwardMotion(struct ScreenTransition *p);
s32 __divsi3(s32, s32);

void SceneActorSequence_UpdateFadeIn(struct ScreenTransition *p)
{
    s32 v;

    {
        s32 duration = 160;
        s32 timer = p->timer;
        if (timer <= 0) {
            v = 320;
        } else if (timer >= duration) {
            v = 192;
        } else {
            s32 k = -32768;
            k *= timer;
            k = __divsi3(k, duration);
            if (k >= 0)
                k >>= 8;
            else
                k = -((-k) >> 8);
            v = k + 320;
        }
        p->x = v;
    }

    {
        s32 duration = 160;
        s32 timer = p->timer;
        if (timer <= 0) {
            v = 224;
        } else if (timer >= duration) {
            v = 32;
        } else {
            s32 k = -49152;
            k *= timer;
            k = __divsi3(k, duration);
            if (k >= 0)
                k >>= 8;
            else
                k = -((-k) >> 8);
            v = k + 224;
        }
        p->y = v;
    }

    {
        s32 n = p->timer + 1;
        p->timer = n;
        if (n > 159)
            ScreenTransition_BeginDownwardMotion(p);
    }
}
