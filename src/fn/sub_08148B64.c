// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player.h"
// confidence=low: action pulses a spiral effect while alternating the folded facing.
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
        register u32 mask asm("r6");
        register u32 temp asm("r1");
        u8 savedDuration;
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
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateFacingSide(struct Player *);
extern u8 *gUnk_030042E4;
void Sound_PlaySong(u32);
void Player_UpdateSpiralDisorientation(struct Player *p)
{
    if (Player_ConsumeStateEntry(p) || !p->tetherFrame) {
        if (p->side)
            p->facing = 7 - p->heading * 2;
        else
            p->facing = p->heading * 2 + 1;
        p->actionEighteenDuration = 0;
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
        Sound_PlaySong(0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    {
        u32 mask = 0x100000;
        p->statusFlags = mask | p->statusFlags;
    }
    if ((p->tetherFrame & 7) < 7 - (p->tetherFrame >> 2)) {
        if (p->facing > 3) {
            p->heading = (7 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
    } else {
        struct Words {
            u32 a, b;
        };
        Player_AddControlFlags(p, 0x41);
        *(struct Words *)((u8 *)p + 0xAC) = *(struct Words *)((u8 *)p + 0x30);
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
    }
    Player_SetAnimation(p, 0x9A, 64, 1);
    p->spriteResource = 0x16F;
    p->actionVariantResource = (u32)(gUnk_030042E4 + (p->spriteResource << 5));
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *(const void **)((u8 *)p + 0x40C), 0,
                     p->heading, p->side);
    Step(&((struct ReleaseAnimation *)p)->sprite, &((struct ReleaseAnimation *)p)->seq);
    {
        register u32 offset asm("r2") = 0xAD6;
        register u16 *parameter asm("r1");
        asm("" : "+r"(offset));
        parameter = (u16 *)((u8 *)p + offset);
        {
            register u32 value = 0x2DD;
            *parameter = value;
        }
    }
    {
        register u32 durationOffset asm("r3") =
            (u32) & ((struct Player *)0)->actionEighteenDuration;
        struct Duration {
            u16 actionEighteenDuration;
        } *duration;
        asm("" : "+r"(durationOffset));
        duration = (struct Duration *)((u8 *)p + durationOffset);
        if ((duration->actionEighteenDuration += 3) > 32)
            duration->actionEighteenDuration = 32;
    }
    if (++p->tetherFrame > 31) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        Player_UpdateFacingSide(p);
        ActorSequence_Begin(p, 0, 0);
    }
}
