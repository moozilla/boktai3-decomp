// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player.h"
// confidence=low: provisional action interpretation from retained C and assembly.
#ifdef NONMATCHING
// fuzzy: 97.3%
// Remaining differences: instruction selection and register allocation.
static inline u32 Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st, u32 firstBit)
{
    struct DisplaySequenceState *state = st;
    struct ContactSwarmSprite *sprite = sp;
    const volatile u16 *command;
    u32 duration, flags, bit;
    u32 ended = 0;
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
        u32 maskBit = firstBit;
        bit = maskBit;
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
                        ended = 1;
                    } else {
                        state->index = state->frameCount - 1;
                        ended = 1;
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
                            ended = 1;
                        }
                    } else {
                        state->index = 0;
                        ended = 1;
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
    return ended;
}

u32 Player_ConsumeStateEntry(struct Player *);
void Player_AddControlFlags(struct Player *, u32);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
// confidence=low: decays the stored recovery amount before playing the finishing pose.
void Player_UpdatePoseRecovery(struct Player *p)
{
    u32 phase;
    Player_AddControlFlags(p, 8);
    phase = *((u8 *)p + 0x458);
    switch (phase) {
        case 0:
            p->facing |= 1;
            if (p->facing > 4) {
                p->heading = (8 - p->facing) >> 1;
                p->side = 1;
            } else {
                p->heading = p->facing >> 1;
                p->side = 0;
            }
            Anim_SetSequence((void *)((u8 *)p + 0x3FC), *(void **)((u8 *)p + 0x410), 2, p->heading,
                             p->side);
            Step((void *)((u8 *)p + 0x14C), (void *)((u8 *)p + 0x3FC), 1);
            if (*(u16 *)((u8 *)p + 0x4EC) <= 30) {
                *(u16 *)((u8 *)p + 0x4EC) = 0;
                Sound_PlaySong(0x1B0);
                *((u8 *)p + 0x458) = 1;
            } else
                *(u16 *)((u8 *)p + 0x4EC) -= 30;
            break;
        case 1:
            Anim_SetSequence((void *)((u8 *)p + 0x3FC), *(void **)((u8 *)p + 0x410), 3, p->heading,
                             p->side);
            if (Step((void *)((u8 *)p + 0x14C), (void *)((u8 *)p + 0x3FC), phase)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Player_UpdatePoseRecovery);
#endif
