#include "global.h"

#define SceneActor_UpdateAnimationDTimer sub_081F612C

typedef struct TimedSceneActor2 {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, padB0, enter, padB2, locked;
    u8 padB4[0x18];
    u8 savedAngle;
    u8 padCD[0xe37];
    s32 duration;
} TimedSceneActor;
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
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
void SceneActor_UpdateAnimationDTimer(TimedSceneActor *p, s32 elapsed)
{
    if (p->enter) {
        p->enter = 0;
        setFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 13, 2, 1, 1);
    }
    if (elapsed > 30)
        p->finished = 1;
}
