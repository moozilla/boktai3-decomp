#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, w;
};
struct Obj {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 tail[8];
};
struct Anim {
    const volatile u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Slot {
    struct Obj sprite;
    struct Anim anim;
    const struct Vec *source;
    u16 stopOnEnd, active;
    u8 pad44[0x1c];
};
struct Manager {
    u8 pad[0x34];
    struct Slot slots[4];
};
static inline u32 frame_factor(u16 v)
{
    u32 value = v;
    u32 mask = 15;
    return mask & value;
}
int PositionEffectDisplay_UpdateSources(struct Manager *m)
{
    struct Cursor {
        u16 frame;
        u8 pad2[10];
        struct Vec position;
        u8 tail[8];
        struct Anim anim;
        const struct Vec *source;
        u16 stopOnEnd, active;
        u8 pad34[0x2c];
    };
    register struct Manager *owner asm("r8") = m;
    register struct Manager *base asm("r12") = owner;
    register s32 offset asm("r10") = 0;
    struct Cursor *cursor = (void *)((u8 *)owner + 0x44);
    struct Anim *sequence = (void *)((u8 *)owner + 0x60);
    struct Obj *sprites = (void *)((u8 *)owner + 0x34);
    do {
        if (cursor->active) {
            struct Manager *p = base;
            struct Obj *o;
            struct Anim *a;
            const volatile u16 *entry;
            register u16 n asm("r0");
            int done;
            {
                u8 *address = (u8 *)owner + 0x70;
                asm("" : "+r"(address));
                cursor->position = **(const struct Vec **)(address + offset);
            }
            o = sprites;
            a = sequence;
            entry = cursor->anim.data + cursor->anim.frame;
            cursor->frame = *entry >> 6;
            {
                u32 input = a->flags;
                register u32 left asm("r2") = 1;
                u32 mask = 1;
                left &= input;
                if (left != (((*entry & 0x30) >> 4) & mask)) {
                    register struct Manager *write asm("r2") = base;
                    asm("" : "+r"(write));
                    write->slots[0].sprite.flags |= 4;
                } else
                    o->flags &= ~4;
            }
            {
                register u32 input asm("r0") = a->flags;
                u32 mask = 2;
                register u32 left asm("r1") = (u8)(mask & input);
                if (left != (((*entry & 0x30) >> 4) & mask))
                    o->flags |= 8;
                else
                    o->flags &= ~8;
            }
            n = ++a->timer;
            if (n >= a->rate) {
                a->timer = 0;
                if (a->flags & 4) {
                    if (a->frame == 0) {
                        if (a->flags & 8) {
                            a->timer = a->rate;
                        } else {
                            a->frame = a->count - 1;
                        }
                        done = 1;
                    } else {
                        a->frame--;
                        done = 0;
                    }
                } else {
                    if (++a->frame >= a->count) {
                        if ((u8)(a->flags & 8)) {
                            a->timer = a->rate;
                            a->frame = a->count - 1;
                        } else {
                            a->frame = 0;
                        }
                        done = 1;
                    } else
                        done = 0;
                }
                entry = a->data + a->frame;
                a->factor = frame_factor(*entry);
                a->rate = (a->factor * a->scale) >> 6;
                if (!a->rate) {
                    u32 one = 1;
                    asm("" : "+r"(one));
                    a->rate = one;
                }
            } else
                done = 0;

            if (done && cursor->stopOnEnd) {
                {
                    u8 *address = (u8 *)owner + 0x34;
                    asm("" : "+r"(address));
                    {
                        register struct Obj *dst asm("r0") = (void *)(address + offset);
                        dst->flags |= 1;
                    }
                }
                cursor->stopOnEnd = 0;
                cursor->active = 0;
            }
        }
        base = (void *)((u8 *)base + 0x60);
        offset += 0x60;
        cursor++;
        sequence++;
        sequence = (void *)((u8 *)sequence + 0x50);
        sprites = (void *)((u8 *)sprites + 0x60);
    } while ((s32)base <= (s32)((u8 *)owner + 0x120));
    return 0;
}
