#include "falling_objects.h"
// MATCHING_CONCESSION: register-pinning
s32 FallingObjectManager_Update(struct FallingObjectManager *manager)
{
    struct FallingObject *p = manager->head, *next;
    struct FallingSprite *o;
    struct FallingAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    register u32 one asm("r9");
    if (p) {
        register u32 initial asm("r0") = 1;
        one = initial;
    }
    for (; p; p = next) {
        next = p->next;
        if (p->remove) {
            sub_0800CC50(p);
            continue;
        }
        if (p->animated) {
            o = &p->sprite;
            a = &p->anim;
            entry = a->data + a->frame;
            o->frame = *entry >> 6;
            {
                u32 input = a->flags;
                register u32 left asm("r2") = (u8)(one & input);
                if (left != (((*entry & 0x30) >> 4) & one))
                    p->sprite.flags |= 4;
                else
                    p->sprite.flags &= ~4;
            }
            {
                register u32 input asm("r0") = a->flags;
                u32 mask = 2;
                u32 left = (u8)(mask & input);
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
                            done = 1;
                        } else {
                            a->frame = 0;
                            done = 1;
                        }
                    } else
                        done = 0;
                }
                entry = a->data + a->frame;
                {
                    register u32 frame asm("r1") = *entry;
                    a->factor = frame & 15;
                }
                a->rate = (a->factor * a->scale) >> 6;
                if (!a->rate)
                    a->rate = one;
            } else
                done = 0;
            p->animationDone = done;
        }
        p->sprite.position = p->position;
        gUnk_08604ACC[p->type][p->state](p);
    }
    return 0;
}
