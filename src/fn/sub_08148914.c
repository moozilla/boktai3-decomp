// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player.h"
static inline void Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration, flags, bit;
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
        register u32 mask asm("r5");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
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
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                } else {
                    state->index = index - 1;
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
                } else {
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
        }
    }
}

struct ReleaseAnimation {
    u8 pad[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad2[0x3FC - 0x14C - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};
u32 Player_ConsumeStateEntry(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_SetActionPhase(struct Player *, s32);
void Sound_PlaySong(u32);
extern const s16 gUnk_086149C4[];
static inline s32 ScaleRecoil(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
// confidence=low: applies a fading impulse opposite the stored facing, then chooses recovery.
void Player_UpdateDecayingRecoil(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (p->contactActionTag) {
            Sound_PlaySong(p->contactActionTag);
            p->contactActionTag = 0;
        }
        Player_SetActionPhase(p, 0);
    }
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *(const void **)((u8 *)p + 0x40C), 1,
                     p->heading, p->side);
    Step(&((struct ReleaseAnimation *)p)->sprite, &((struct ReleaseAnimation *)p)->seq);
    {
        register u32 offset asm("r2") = 0x270;
        u16 *magnitude;
        asm("" : "+r"(offset));
        magnitude = (u16 *)((u8 *)p + offset);
        asm("" : "+r"(magnitude));
        if (*magnitude) {
            register u32 facingOffset asm("r5") = 0x3A4;
            register u8 *facing asm("r0");
            u32 angle;
            asm("" : "+r"(facingOffset));
            facing = (u8 *)p + facingOffset;
            angle = ((*facing + 5) & 7) << 5;
            p->velocityX = -ScaleRecoil(gUnk_086149C4[(angle + 64) & 255] * *magnitude);
            p->velocityZ = -ScaleRecoil(gUnk_086149C4[angle] * p->directionFrame);
            p->directionFrame = (p->directionFrame * 3) >> 2;
        } else {
            p->contactSpecial = 0;
            if (*((u8 *)p + 0x5B9) == 3 && *((u8 *)p + 0x5BB))
                ActorSequence_Begin(p, 22, 0);
            else
                ActorSequence_Begin(p, 0, 0);
        }
    }
}
