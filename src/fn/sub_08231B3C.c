// MATCHING_CONCESSION: register-pinning
#include "global.h"
/* EffectPart_UpdateBlink: advances a sprite part's frame animation, then blinks it (the hide interval shrinks each time) and frees the slot at zero (provisional name) */
#define EffectPart_UpdateBlink sub_08231B3C
#include "homing_effect.h"

struct Unk08231A0CManager {
    u8 unknown0[0x18];
    struct EffectGraphics *graphics;
};
struct Unk08231A0CGroup {
    u8 unknown0[2];
    u8 active;
};
void SceneActorEffects_FinishSlot(void *);
struct Unk08231A0CPart {
    u8 state, phase;
    u16 timer;
    s16 lifetime;
    u8 unknown6[0xA];
    struct EffectSprite sprite;
    struct EffectAnim anim;
};

void EffectPart_UpdateBlink(struct Unk08231A0CManager *manager, struct Unk08231A0CGroup *group,
                            struct Unk08231A0CPart *p)
{
    struct EffectSprite *sprite = &p->sprite, *o;
    struct EffectAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
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
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                    goto reload;
                }
                a->frame = a->count - 1;
            } else
                a->frame--;
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                } else
                    a->frame = 0;
            }
        }
    reload:
        entry = a->data + a->frame;
        a->rate = ((*entry & 15) * a->scale) >> 6;
    }
    if (p->timer == p->phase) {
        p->sprite.flags |= 1;
        if (p->phase)
            p->phase--;
        p->timer = 0;
    } else
        p->sprite.flags &= ~1;
    if (!p->phase) {
        SceneActorEffects_FinishSlot(p);
        group->active--;
    }
}
