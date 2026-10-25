// MATCHING_CONCESSION: register-pinning
#include "global.h"

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
    out->z = TruncateScale(gUnk_086149C4[angle & 255] * scale);
}
static inline void AddVector(struct Vec *dst, struct Vec *a, struct Vec *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline u32 StepPlayback(struct FragmentSprite *owner, struct FragmentSequence *state,
                               struct FragmentSprite *base, u32 one)
{
    struct FragmentSprite *sprite = owner;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    register u32 endMask asm("r12");
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = one;
        bit = maskBit;
        endMask = 8;

        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;

        first = bit;
        first &= input;

        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->flags |= endMask;
        else
            owner->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        u8 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
        asm("" : : "r"(temp), "r"(mask));
        /* Preserve separate mask and duration values. */

        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u16 index = state->index;
                if (index == 0) {
                    u32 hold = endMask;
                    hold &= flags;
                    asm("" : "+r"(hold));
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & endMask;

                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = ((speed * weight) >> 6);
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

/* Tentative: animate four radial burst sprites and advance after the last sequence ends. */
#ifdef NONMATCHING
// fuzzy: 96.5%
// scope shape 6 (differences once low registers are set aside)
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
        ended = StepPlayback(&p->sprites[i], &p->sequences[i], &p->sprites[i], 1);
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
