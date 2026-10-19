#define SceneNpc_InitializeFacingAnimation sub_081D1A44
#define SceneNpc_SelectAnimationEntry      sub_081D1A10
#include "global.h"
#include "state_actor.h"
void sub_081D1A10(void *, int, int, int, int);
void sub_081D1A44(void *ctx, struct StateActor *p)
{
    if (p->enter) {
        s32 angle;
        s32 facing;
        p->enter = 0;
        p->done = 0;
        angle = p->targetAngle;
        p->angle = angle;
        facing = (angle + 0x70) >> 5;
        {
            u8 mask = 7;
            p->facing8 = facing & mask;
        }
        angle += 0x60;
        angle >>= 6;
        {
            u8 mask = 3;
            p->facing4 = angle & mask;
        }
        sub_081D1A10(p, 0, 1, 1, 4);
    }
}
