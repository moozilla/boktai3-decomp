#include "global.h"
#include "actor_attachment_state.h"
static inline u32 ActorAttachment_StepSprite(struct ActorAttachmentPartSprite *sprite,
                                             const struct ActorAttachmentPartResource *resource,
                                             struct DisplaySequenceState *a)
{
    const volatile u16 *entry;
    u32 done;
    {
        u32 offset = a->index * 2;
        const u16 *table = a->commands;
        entry = (const volatile u16 *)((u32)table + offset);
    }
    {
        u32 frame = *entry >> 6;
        u32 tiles = (sprite->width >> 3) * (sprite->height >> 3);
        sprite->tile = tiles * frame + resource->tileBase;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++a->elapsed >= a->duration) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->index == 0) {
                if (a->flags & 8)
                    a->elapsed = a->duration;
                else
                    a->index = a->frameCount - 1;
                done = 1;
            } else {
                --a->index;
                done = 0;
            }
        } else {
            if (++a->index >= a->frameCount) {
                if ((u8)(a->flags & 8)) {
                    a->elapsed = a->duration;
                    a->index = a->frameCount - 1;
                } else
                    a->index = 0;
                done = 1;
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        {
            u32 duration = *entry & 15;
            a->duration = ((s32)duration * a->speed) >> 6;
        }
    } else
        done = 0;
    return done;
}

struct Particle {
    struct ContactPosition position;
    s16 dx, dy;
    u8 padC[6];
    u8 state;
    s8 index;
    u16 timer;
};
struct Manager {
    u8 pad[0x1c];
    struct ActorAttachmentPartResource *resource;
    struct ActorAttachmentPartSprite sprites[32];
    struct DisplaySequenceState sequences[32];
};
void NativeParticle_UpdateAnimatedMotion(struct Manager *m, struct Particle *p)
{
    struct ActorAttachmentPartSprite *sprite = &m->sprites[p->index];
    sprite->flags &= ~1;
    p->position.x += p->dx;
    p->position.y += p->dy;
    sprite->position = p->position;
    if (ActorAttachment_StepSprite(sprite, m->resource, &m->sequences[p->index])) {
        p->state = 2;
    }
    {
        p->timer++;
    }
}
