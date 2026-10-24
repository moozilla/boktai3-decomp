#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad0[7];
    u8 done;
    u8 pad8[1];
    u8 entered;
    u8 pada[4];
    u16 flags;
    u8 pad10[8];
    u8 phase;
    u8 facing4;
    u8 facing8;
    u8 targetAngle;
    u8 pad1c[56];
    struct Vec target;
    u8 body[9];
    u8 angle;
    u8 pad66[2];
    struct Vec pos;
    struct Vec velocity;
    u8 pad78[92];
    u32 drawable;
    u8 padd8[80];
    struct Vec *sourcePos;
    u8 pad12c[52];
    struct Vec *drawPos;
    u8 pad164[164];
    int speed;
    u32 speedSquared;
    u8 pad210[212];
    int (*callback)(struct Actor *);
};

static inline s8 *SceneActorAnimation_SetAngle(struct Actor *p, s16 angle)
{
    u16 facing;
    p->angle = angle;
    facing = angle;
    {
        s32 shifted = (facing + 112) >> 5;
        u32 mask = 7;
        mask &= shifted;
        p->facing8 = mask;
    }
    {
        s32 shifted = (facing + 96) >> 6;
        u32 mask = 3;
        mask &= shifted;
        p->facing4 = mask;
    }
    return (s8 *)&p->angle;
}

int Actor_PlayAnimationEntry(struct Actor *, int, int, int, int);
// confidence=low: turns toward a scripted angle and signals completion when aligned.
#define SceneActorAnimation_UpdateTurnToAngle sub_0802EBD0
void SceneActorAnimation_UpdateTurnToAngle(void *unused, struct Actor *p)
{
    if (p->entered) {
        p->entered = 0;
        p->done = 0;
        Actor_PlayAnimationEntry(p, 2, 1, 0, 4);
    }
    {
        u32 mask = 0x1000;
        p->flags |= mask;
    }
    if (((u8)p->callback(p)) != 1) {
        s8 *angle = (s8 *)&p->angle;
        s8 current = *angle;
        s32 delta = (s8)(p->targetAngle - current);
        s8 *savedAngle;
        if (delta > 8)
            current = (s8)current + 8;
        else if (delta < -8)
            current = (s8)current - 8;
        else
            current = (s8)current + delta;
        savedAngle = SceneActorAnimation_SetAngle(p, current);
        Actor_PlayAnimationEntry(p, 2, 1, 0, 1);
        if ((u8)*savedAngle == p->targetAngle)
            p->done = 1;
    }
}
