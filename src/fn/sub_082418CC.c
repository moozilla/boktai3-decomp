// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"

/* Partial view of the player fields used by the recovery pose. */
struct PlayerCue {
    u8 unk_00[0x18];
    s32 slot;
    u8 unk_1C[0x14C - 0x1C];
    struct ContactSwarmSprite sprite;
    u8 unk_178[0x3A2 - 0x178];
    u8 heading, side, facing;
    u8 unk_3A5[0x3FC - 0x3A5];
    struct DisplaySequenceState sequence;
    u32 unk_40C;
    void *resource;
    u8 unk_414[0x458 - 0x414];
    u8 phase;
    u8 unk_459[0x4EC - 0x459];
    u16 countdown;
};
// confidence=low: provisional action interpretation from retained C and assembly.
static inline u32 Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st, u32 firstBit,
                       u32 pin)
{
    struct DisplaySequenceState *state = st;
    struct ContactSwarmSprite *sprite = sp;
    const volatile u16 *command;
    u32 duration, flags, bit;
    u32 ended;
    u32 endFlagTmp;
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
        if (pin == 1)
            asm("" : "+r"(bit));
        if (pin == 2) {
            u32 f2;
            u32 inp = input;
            asm("" : "=r"(f2) : "0"(bit));
            f2 &= inp;
            if (f2 != (((*command & 0x30) >> 4) & bit))
                sprite->flags |= 4;
            else
                sprite->flags &= ~4;
            goto done1;
        }
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    done1:;
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
                        goto setEnded;
                    } else {
                        endFlagTmp = state->frameCount - 1;
                        goto storeIndex;
                    }
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
                        endFlag = frameCount - 1;
                    }
                    endFlagTmp = endFlag;
                storeIndex:
                    state->index = endFlagTmp;
                setEnded:
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
                s32 speed;
                speed = (state->frameWeight * state->speed) >> 6;
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

u32 Player_ConsumeStateEntry(struct PlayerCue *);
void Player_SetSecondaryActionFlags(struct PlayerCue *, u32);
void sub_08238238(struct PlayerCue *);
void Player_SetTetherMode(struct PlayerCue *, s32, s32);
void Player_PlayLocalSong(s32, u32);
// confidence=low: decays the stored recovery amount before playing the finishing pose.
void PlayerCue_UpdatePoseRecovery(struct PlayerCue *p)
{
    u32 phase;
    Player_SetSecondaryActionFlags(p, 8);
    phase = p->phase;
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
            Anim_SetSequence(&p->sequence, p->resource, 2, p->heading, p->side);
            Step(&p->sprite, &p->sequence, 1, 1);
            if (p->countdown <= 30) {
                p->countdown = 0;
                Player_PlayLocalSong(p->slot, 0x1B0);
                p->phase = 1;
            } else
                p->countdown -= 30;
            break;
        case 1:
            Anim_SetSequence(&p->sequence, p->resource, 3, p->heading, p->side);
            if (Step(&p->sprite, &p->sequence, phase, 2)) {
                sub_08238238(p);
                Player_SetTetherMode(p, 0, 0);
            }
            break;
    }
}
