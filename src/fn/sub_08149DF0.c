// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "contact_swarm.h"
// confidence=low: action creates the spiral trail and advances its auxiliary sprite.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void Sound_PlaySong(u32);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
extern u8 *gUnk_030042E4;
void ActorSequence_Begin(struct Player *, s32, s32);
static inline void addFlags(u32 *flags, u32 mask)
{
    *flags |= mask;
}
// Two address-selection instructions still use r0 instead of r3.
#define Player_UpdateSpiralTrailAction sub_08149DF0
#ifdef NONMATCHING
// fuzzy: 99.3%
void Player_UpdateSpiralTrailAction(struct Player *p)
{
    struct ContactSwarmSprite *sprite;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    if (Player_ConsumeStateEntry(p) || !p->tetherFrame) {
        p->actionEighteenDuration = 0;
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
        Sound_PlaySong(0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    addFlags((u32 *)((u8 *)p + 0x20), 0x100000);
    if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 2)) {
        Player_AddControlFlags(p, 0x41);
        p->previousPosition = p->position;
    }
    Player_SetAnimation(p, 0x9A, 64, 1);
    {
        register u16 *resource asm("r0") = &p->spriteResource;
        register u32 id asm("r1") = 0x16F;
        *resource = id;
        asm("" : "+r"(id));
        p->actionVariantResource = (u32)(gUnk_030042E4 + id * 32);
    }
    state = (void *)((u8 *)p + 0x3FC);
    Anim_SetSequence(state, *(void **)((u8 *)p + 0x410), 0, p->heading, p->side);
    sprite = (void *)((u8 *)p + 0x14C);
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = bit;
        asm("" : "+r"(first));
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
    {
        register u32 offset asm("r2") = 0xAD6;
        register u16 *target asm("r1");
        register u32 value asm("r0");
        asm("" : "+r"(offset));
        target = (u16 *)((u8 *)p + offset);
        asm("" : "+r"(target));
        value = 0x16F;
        asm("" : "+r"(value));
        *target = value;
    }
    if ((p->actionEighteenDuration += 3) > 32)
        p->actionEighteenDuration = 32;
    if (++p->tetherFrame > 31) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        ActorSequence_Begin(p, 0, 0);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateSpiralTrailAction);
#endif
