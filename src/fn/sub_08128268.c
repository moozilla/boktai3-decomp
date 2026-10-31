#include "global.h"
#include "display_sequence.h"
#include "contact_position.h"

struct FragmentSprite {
    u32 flags;
    u8 unk_04[0xc];
    u16 frame;
    u8 unk_12[0xa];
    struct ContactPosition position;
    u8 unk_24[8];
};
struct FragmentBurst {
    struct FragmentSprite sprites[4];
    struct DisplaySequenceState sequences[4];
    struct ContactPosition origin, position;
    u8 collider[0x54];
    u16 age;
    u8 state;
    s8 slot;
    u32 completionScript;
    u16 timer;
    u8 enter;
};
struct FragmentBurstPool {
    u8 unk_00[0x34];
    void *sequences;
};
extern const s16 gUnk_086149C4[];
void Anim_SetSequence(void *, void *, u32, u32, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
static inline s32 FragmentBurst_TruncateScale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void FragmentBurst_MakeRadialOffset(struct ContactPosition *out, s32 angle, s32 scale)
{
    out->x = FragmentBurst_TruncateScale(gUnk_086149C4[(angle + 64) & 255] * scale);
    out->y = 0;
    {
        u32 masked = 255;
        masked &= angle;
        out->z = FragmentBurst_TruncateScale(gUnk_086149C4[masked] * scale);
    }
}
static inline void FragmentBurst_AddPosition(struct ContactPosition *dst, struct ContactPosition *a,
                                             struct ContactPosition *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline u32 FragmentBurst_StepPlayback(struct FragmentSprite *owner,
                                             struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = state->index << 1;
        const u16 *table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        {
            u32 sample = *command;
            u32 weight = 15;
            weight &= sample;
            state->frameWeight = weight;
        }
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

static inline struct DisplaySequenceState *
FragmentSequence_Prepare(struct FragmentBurst *const *owner, struct FragmentBurstPool *pool,
                         u32 flipX, u32 flipY, void **resource, u32 *mirrors)
{
    struct DisplaySequenceState *base = (*owner)->sequences;
    *resource = pool->sequences;
    *mirrors = flipX | flipY;
    return base;
}

/* Tentative: animate four radial burst sprites and advance after the last sequence ends. */
void FragmentBurst_UpdateRadialAnimation(struct FragmentBurstPool *pool, struct FragmentBurst *p)
{
    s32 i;
    u32 ended;
    if (p->enter) {
        struct ContactPosition offset;
        struct FragmentSprite *sprite = p->sprites;
        struct FragmentBurst *sequenceOwner = p;
        for (i = 0; i < 4; i++) {
            u32 flipX, flipY;
            sprite->flags &= ~0x10;
            sprite->position = p->position;
            FragmentBurst_MakeRadialOffset(&offset, i << 6, 128);
            FragmentBurst_AddPosition(&sprite->position, &sprite->position, &offset);
            switch (i) {
                case 0:
                    flipX = 2;
                    flipY = 0;
                    break;
                case 1:
                    flipX = 2;
                    flipY = 1;
                    break;
                case 2:
                    flipX = 0;
                    flipY = 1;
                    break;
                default:
                    flipX = 0;
                    flipY = 0;
                    break;
            }
            {
                void *resource;
                u32 mirrors;
                struct DisplaySequenceState *base = FragmentSequence_Prepare(
                    &sequenceOwner, pool, flipX, flipY, &resource, &mirrors);
                Anim_SetSequence(&base[i], resource, 16, 0, mirrors);
            }
            sprite->flags &= ~1;
            sprite++;
        }
        p->timer = 0;
        p->enter = 0;
    }
    for (i = 0; i < 4; i++)
        ended = FragmentBurst_StepPlayback(&p->sprites[i], &p->sequences[i]);
    Collider_SetPosition(p->collider, &p->position, 0);
    Collider_Refresh(p->collider);
    if (ended) {
        p->state = 3;
        p->enter = 1;
    } else {
        p->timer++;
    }
}
