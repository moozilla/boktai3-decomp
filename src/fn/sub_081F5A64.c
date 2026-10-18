#define SceneActor_UpdateAnimation33Timer sub_081F5A64
#include "global.h"
typedef struct TimedSceneActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, padB0, enter, padB2, locked;
    u8 padB4[0x60];
    s8 savedAngle;
    u8 pad115[0xdef];
    s32 duration;
} TimedSceneActor;
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
static inline void setFacing(TimedSceneActor *p, int angle)
{
    int shifted;
    u32 value;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}
void sub_081F5A64(TimedSceneActor *p, s32 elapsed)
{
    if (p->enter) {
        p->enter = 0;
        if (p->locked)
            setFacing(p, p->savedAngle);
        SceneActors_StartAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 33, 1, 4);
    }
    if (elapsed > 16)
        p->finished = 1;
}
