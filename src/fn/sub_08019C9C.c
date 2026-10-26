#include "global.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
typedef struct {
    u16 *data;
    u8 flags, frames, weight, duration;
    u16 frame, pad, speed, time;
} Anim;
typedef struct {
    u32 flags;
    u8 pad[12];
    u16 tile;
    u8 tail[54];
} Sprite;
typedef struct {
    u32 id;
    u8 pad[8];
    Sprite sprite;
    Anim anim;
} Slot;
int SpritePool_AdvanceAnimation(Slot *arg)
{
    register Slot *p asm("ip") = arg;
    Sprite *sprite;
    Anim *a;
    u16 *frame;
    int done;
    if (!gSpritePoolManager)
        return 0;
    sprite = &p->sprite;
    a = &p->anim;
    {
        int offset = a->frame * 2;
        register Slot *base asm("r2");
        base = p;
        frame = (u16 *)((u32)base->anim.data + offset);
    }
    sprite->tile = *frame >> 6;
    {
        int v = a->flags;
        register int m asm("r3") = 1;
        int left;
        asm("" : "+r"(v), "+r"(m));
        left = m & v;
        asm("" : "+r"(left));
        {
            register u32 flags asm("r0");
            if (left != ((*frame & 0x30) >> 4 & m)) {
                register Slot *base asm("r1") = p;
                flags = base->sprite.flags;
                flags |= 4;
            } else {
                register Slot *base asm("r1") = p;
                flags = base->sprite.flags;
                flags &= ~4;
            }
            {
                register Slot *base asm("r2") = p;
                base->sprite.flags = flags;
            }
        }
    }
    {
        int v = a->flags;
        register int m asm("r3") = 2;
        register int left asm("r1");
        left = m & v;
        left = (u8)left;
        if (left != ((*frame & 0x30) >> 4 & m))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++a->time >= a->duration) {
        a->time = 0;
        if (a->flags & 4) {
            if (!a->frame) {
                if (a->flags & 8)
                    a->time = a->duration;
                else
                    a->frame = a->frames - 1;
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->frames) {
                if ((u8)(a->flags & 8)) {
                    a->time = a->duration;
                    a->frame = a->frames - 1;
                } else
                    a->frame = 0;
                done = 1;
            } else
                done = 0;
        }
        frame = a->data + a->frame;
        {
            register u16 v asm("r1") = *frame;
            a->weight = v & 15;
        }
        a->duration = (a->weight * a->speed) >> 6;
        if (!a->duration)
            a->duration = 1;
    } else
        done = 0;
    return done;
}
