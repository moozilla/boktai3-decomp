// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
#ifdef NONMATCHING
// fuzzy: 95.9%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences are loop setup, mask allocation and short-lived register copies.
struct Vec {
    s16 x, y, z, pad;
};
struct FragmentSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct FragmentSprite {
    u32 flags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct Vec position;
    u8 unknown24[8];
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
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
static inline s32 ProjectScale(s32 n)
{
    s32 v = n * 3;
    if (v >= 0)
        v >>= 5;
    else
        v = -(-v >> 5);
    return v;
}
static inline s32 ProjectHeight(s32 n)
{
    s32 v = n * 3;
    if (v >= 0)
        return v >> 5;
    return -(-v >> 5);
}
static inline void Project(struct Vec *out, struct Vec *in)
{
    s32 x = in->x, z = in->z, h, y;
    out->x = ProjectScale(x - z);
    h = ProjectScale(x + z);
    y = ProjectHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
static inline u32 Step(struct FragmentSprite *owner, struct FragmentSequence *state,
                       struct FragmentSprite *base, u32 endMask)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = 1;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & 1))
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
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= endMask;
        else
            owner->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r2");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */

        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = endMask;
                    hold &= flags;
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
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

// confidence=low: animates three projected burst sprites and enables their short-lived impact collider.
static inline void ProjectShift(struct Vec *out, struct Vec *in, struct Vec *camera)
{
    Project(out, in);
    out->x = out->x - camera->x + 120;
    out->y = out->y - camera->y + 80;
    out->z -= camera->z;
}
inline static u32 Step_UpdateImpactAnimation(struct FragmentSprite *owner,
                                             struct FragmentSequence *state,
                                             struct FragmentSprite *base, u32 endMask)
{
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    owner->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = 1;
        first &= input;
        {
            u8 k1 = 1;
            if (first != ((((*command) & 0x30) >> 4) & k1))
                owner->flags |= 4;
            else
                owner->flags &= ~4;
        }
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        asm("" : "+r"(first));
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
        register u32 savedDuration asm("r2");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = endMask;
                    hold &= flags;
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
                speed = (speed * weight) >> 6;
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

void FragmentBurst_UpdateImpactAnimation(void *unused, struct FragmentBurst *p)
{
    s32 i;
    struct Vec *camera;
    u32 endMask;
    if (p->enter) {
        Sound_PlaySong(0x3B1);
        p->enter = 0;
    }
    i = 0;
    endMask = 8;
    camera = &gCameraOrigin;
    do {
        struct FragmentSprite *sprite = &p->sprites[i];
        sprite->flags &= ~1;
        Step_UpdateImpactAnimation(sprite, &p->sequences[i], sprite, endMask);
        ProjectShift(&sprite->position, &p->origin, camera);
        sprite->position.y += i * 32;
        i++;
    } while (i < 3);
    if (p->timer > 4) {
        p->state = 2;
        p->enter = 1;
    } else {
        Collider_SetPosition(p->collider, &p->position, 0);
        Collider_Refresh(p->collider);
        p->timer++;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", FragmentBurst_UpdateImpactAnimation);
#endif
