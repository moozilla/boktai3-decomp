#include "global.h"
struct Actor {
    u8 pad0[0xb2];
    u8 active;
    u8 padb3[3];
    u8 mode, speed;
    u8 padb8[0x7fc];
    u8 animation[16];
    void *resource;
};
void Anim_RestartSequence(void *, void *, int, int, int);
void Anim_SetSequence(void *, void *, int, int, int);
void GridAttackActor_SetAnimation(struct Actor *p, int sequence, int start, int speed)
{
    p->mode = 0;
    p->speed = speed;
    if (start) {
        Anim_RestartSequence(p->animation, p->resource, (u16)sequence, p->mode, p->speed);
        p->active = 0;
    } else
        Anim_SetSequence(p->animation, p->resource, (u16)sequence, p->mode, p->speed);
}
