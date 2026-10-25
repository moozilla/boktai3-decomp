// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "actor_attachment_state.h"

struct FollowingPosition {
    s32 x, y, z, w;
};
struct FollowingCamera {
    u8 unk_00[0x44];
    s32 x;
};
struct FollowingEffect {
    void (*update)(struct FollowingEffect *);
    u8 collider[0x5c];                /* 0x04 */
    struct ContactSwarmSprite sprite; /* 0x60 */
    struct DisplaySequenceState anim; /* 0x8c */
    void *sequence;
    struct FollowingPosition *anchor;  /* 0xa0 */
    struct FollowingPosition position; /* 0xa4 */
    u8 unk_B4[0x10];
    s32 *height; /* 0xc4 */
    u8 unk_C8[4];
    u16 duration;
    u8 active, entered;
};
extern struct FollowingCamera *gUnk_020004B8;
s32 Div(s32, s32);
void Collider_SetFixedPositionWithYOffset(void *, struct FollowingPosition *, s32);
void SpatialIndex_InsertEntry(void *);
void FollowingSpriteEffect_SetStateCallback(struct FollowingEffect *,
                                            void (*)(struct FollowingEffect *));
void FollowingSpriteEffect_OnStop(struct FollowingEffect *);

static inline u32 FollowingSpriteEffect_StepSprite(struct ContactSwarmSprite *sprite,
                                                   struct DisplaySequenceState *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
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
                    endFlag = flags & 8;
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
/* Tentative: ease toward the anchor while playing the attached sprite. */
void FollowingSpriteEffect_UpdateFollowing(struct FollowingEffect *p)
{
    struct FollowingPosition target;
    s32 delta, height, pixels;
    struct FollowingPosition *pos;
    struct ContactPosition *dst;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->position = *p->anchor;
        p->entered = 0;
    }
    p->position.y = p->anchor->y;
    p->position.z = p->anchor->z + 0x12000;
    target.x = p->anchor->x + 0x40000;
    delta = Div(target.x - p->position.x, 8);
    pixels = delta >> 12;
    pos = &p->position;
    if (pixels > 6)
        delta = Div(target.x - pos->x, 4);
    else if (pixels > 4)
        delta = Div(target.x - pos->x, 6);
    pos->x += delta;
    height = *p->height - 0x20000;
    dst = &p->sprite.position;
    dst->x = (pos->x >> 12) - gUnk_020004B8->x;
    dst->y = ((s32)(pos->z + (pos->y + height)) >> 12) + 0x60;
    dst->z = pos->z >> 12;
    FollowingSpriteEffect_StepSprite(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, pos, height);
    SpatialIndex_InsertEntry(p->collider);
    if (!--p->duration)
        FollowingSpriteEffect_SetStateCallback(p, FollowingSpriteEffect_OnStop);
}
