#include "global.h"
#include "falling_objects.h"
// MATCHING_CONCESSION: register-pinning
struct ColliderSpriteObject {
    u8 unknown0[2];
    u8 type, unknown3;
    u8 animationDone, remove;
    u8 unknown6[6];
    struct FallingVec position;
    u8 unknown14[0x28];
    struct FallingSprite sprite;
    struct FallingAnim anim;
    u8 unknown78[4];
    struct ColliderSpriteObject *next;
};
struct ColliderSpritePool {
    u8 task[0x1C];
    struct ColliderSpriteObject *head;
};
extern void *const gUnk_08E879CC[];
void ColliderSprite_Destroy(void *);
s32 Callback_InvokeActor(void *, void *);

s32 ColliderSpritePool_Update(struct ColliderSpritePool *pool)
{
    struct ColliderSpriteObject *p = pool->head, *next;
    struct FallingSprite *o;
    struct FallingAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    u32 one;
    if (p) {
        u32 initial = 1;
        one = initial;
    }
    for (; p; p = next) {
        next = p->next;
        if (p->remove) {
            ColliderSprite_Destroy(p);
            continue;
        }
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
        p->sprite.position = p->position;
        Callback_InvokeActor(p, gUnk_08E879CC[p->type]);
    }
    return 0;
}
