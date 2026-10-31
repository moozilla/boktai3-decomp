#include "global.h"
typedef struct A {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished;
    u8 enter;
    u8 changed;
    u8 pad[0x114 - 0xB2];
    s8 savedAngle;
    u8 pad115[0x2AC - 0x115];
    u8 animationState[0x20];
    u8 animation[1];
} A;

static inline void setFacing(A *p, int angle)
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
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void sub_0817F330(A *p)
{
    A *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    if (*(u8 *)((u8 *)p + 0xb1)) {
        *(u8 *)((u8 *)p + 0xb1) = 0;
        SceneActors_StartEightDirectionAnimation(p, p->animation, p->animationState, 0xF4, 1, 1, 4);
    } else {
        SceneActors_StartEightDirectionAnimation(p, p->animation, p->animationState, 0xF4, 1, 1, 1);
    }
    {
        u32 mask = 0x400004;
        p2->flags |= mask;
    }
    {
        int delta = (s8)(position[9] - (u8)p2->savedAngle), distance = delta;
        if (distance < 0)
            distance = -distance;
        if (distance <= 7) {
            setFacing(p2, p2->savedAngle);
            p2->finished = 1;
        } else {
            int angle;
            if (delta < 0)
                angle = position[9] + 8;
            else
                angle = position[9] - 8;
            setFacing(p2, angle);
        }
    }
}
