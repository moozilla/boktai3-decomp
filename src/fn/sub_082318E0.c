// MATCHING_CONCESSION: register-pinning
#include "global.h"
/* EffectPart_UpdateFinish (tentative): advances the animation and releases the slot at its end. */
#include "homing_effect.h"

struct Unk08231A0CManager {
    u8 unk_00[0x18];
    struct EffectGraphics *graphics;
};
struct Unk08231A0CGroup {
    u8 unk_00[2];
    u8 active;
};
void SceneActorEffects_FinishSlot(void *);
struct Unk08231A0CPart {
    u8 state, phase;
    u16 timer;
    s16 lifetime;
    u8 unk_06[0xA];
    struct EffectSprite sprite;
    struct EffectAnim anim;
};

void EffectPart_UpdateFinish(struct Unk08231A0CManager *manager, struct Unk08231A0CGroup *group,
                  struct Unk08231A0CPart *p)
{
    struct EffectSprite *sprite = &p->sprite, *o;
    struct EffectAnim *a;
    const u16 *entry;
    u32 n;
    u8 done;
    o = sprite;
    {
        struct EffectGraphics *graphics = manager->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
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
    if ((u16)n >= a->rate) {
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
        a->rate = ((*entry & 15) * a->scale) >> 6;
    } else
        done = 0;
    if (done) {
        SceneActorEffects_FinishSlot(p);
        group->active--;
    }
}
