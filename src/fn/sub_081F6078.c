#include "global.h"
typedef struct TimedSceneActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x56];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished;
    u8 padB0;
    u8 changed;
    u8 padB2[0x1a];
    u8 requestedAngle;
    u8 padCD[0x47];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 motion;
    u8 pad180[0xcd2];
    s16 fieldE52, fieldE54;
    u8 padE56[2];
    s16 fieldE58;
} TimedSceneActor;
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
static inline void setFacing(TimedSceneActor *p, int angle)
{
    p->angle = angle;
    {
        int shifted = (p->angle + 0x60) >> 6;
        u32 value = 3;
        value &= shifted;
        p->facing = value;
    }
    {
        int shifted = (p->angle + 0x70) >> 5;
        u32 value = 7;
        value &= shifted;
        p->facing8 = value;
    }
}
void SceneActors_UpdateTimedAnimationReset(TimedSceneActor *p, int tick)
{
    if (p->changed) {
        u32 zero = 0;
        p->changed = zero;
        setFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 26, 1, 1, 4);
        p->motion = zero;
        p->fieldE52 = zero;
        p->fieldE54 = zero;
        p->fieldE58 = zero;
    }
    if (tick == 35) {
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 17, 0, 4, 4);
        p->finished = 1;
    }
}
