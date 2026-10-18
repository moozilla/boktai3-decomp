#define SceneActors_UpdateTimedDirectionalAnimation sub_081F5FEC
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
} TimedSceneActor;
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void sub_081F5FEC(TimedSceneActor *p, int tick)
{
    if (p->changed) {
        p->changed = 0;
        p->angle = p->requestedAngle;
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
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 13, 1, 1, 4);
    }
    if (tick > 19) {
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 13, 0, 1, 1);
        p->finished = 1;
    }
}
