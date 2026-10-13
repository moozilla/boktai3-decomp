// MATCHING_CONCESSION: register-pinning
#include "global.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[7];
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    void (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    u16 scale, minScale, maxScale;
    u8 pad34A[0x62];
    u32 mode;
} ActorAnimation;
static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void sub_08024E18(ActorAnimation *p)
{
    if (!p->locked)
        setFacing(p, p->requestedAngle);
    if (p->changed) {
        int sequence;
        p->changed = 0;
        if (p->locked)
            setFacing(p, p->savedAngle);
        {
            int test = p->mode & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        SceneActors_StartEightDirectionAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1,
                                                 1, 4);
    } else {
        int sequence;
        {
            int test = p->mode & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        SceneActors_StartEightDirectionAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 1,
                                                 1, 1);
    }
    if (!p->tryUpdate(p)) {
        u32 mask = 0x40;
        p->flags |= mask;
        p->reset(p);
    }
}
