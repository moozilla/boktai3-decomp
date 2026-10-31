#include "global.h"
#include "display_sequence.h"

struct Vec {
    s16 x, y, z, pad;
};
struct FragmentSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unk_0A, speed, elapsed;
};
struct FragmentSprite {
    u32 flags;
    u8 unk_04[0xc];
    u16 frame;
    u8 unk_12[0xa];
    struct Vec position;
    u8 unk_24[8];
};
struct FragmentBurst {
    struct FragmentSprite sprites[4];
    struct FragmentSequence sequences[4];
    struct Vec origin, position;
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
static inline s32 TruncateScale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void MakeVector(struct Vec *out, s32 angle, s32 scale)
{
    out->x = TruncateScale(gUnk_086149C4[(angle + 64) & 255] * scale);
    out->y = 0;
    {
        u32 masked = 255;
        masked &= angle;
        out->z = TruncateScale(gUnk_086149C4[masked] * scale);
    }
}
static inline void AddVector(struct Vec *dst, struct Vec *a, struct Vec *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline u32 StepPlayback(struct FragmentSprite *owner, struct FragmentSequence *state)
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

/* Tentative: animate four radial burst sprites and advance after the last sequence ends. */
#ifdef NONMATCHING
// fuzzy: 99.7%
void FragmentBurst_UpdateRadialAnimation(struct FragmentBurstPool *pool, struct FragmentBurst *p)
{
    s32 i;
    u32 ended;
    if (p->enter) {
        struct Vec offset;
        struct FragmentSprite *sprite = p->sprites;
        for (i = 0; i < 4; i++) {
            u32 flipX, flipY;
            sprite->flags &= ~0x10;
            sprite->position = p->position;
            MakeVector(&offset, i << 6, 128);
            AddVector(&sprite->position, &sprite->position, &offset);
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
            Anim_SetSequence(&p->sequences[i], pool->sequences, 16, 0, flipX | flipY);
            sprite->flags &= ~1;
            sprite++;
        }
        p->timer = 0;
        p->enter = 0;
    }
    for (i = 0; i < 4; i++)
        ended = StepPlayback(&p->sprites[i], &p->sequences[i]);
    Collider_SetPosition(p->collider, &p->position, 0);
    Collider_Refresh(p->collider);
    if (ended) {
        p->state = 3;
        p->enter = 1;
    } else {
        p->timer++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", FragmentBurst_UpdateRadialAnimation);
#endif
