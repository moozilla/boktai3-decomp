#include "contact_swarm.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: walking animation alternates footstep sounds and positional effects.

u32 Anim_SetSequence(void *anim, void *data, u32 sequence, u32 row, u32 flags);
s32 RepeatedEffect_RequestNearby(void *);
void Sound_PlaySong(u32 song);
void Player_UpdateFacingSide(void *);
u32 Player_ConsumeStateEntry(u8 *p);
u8 Player_GetSmoothedDirection(void *);
s32 Player_GetWalkSpeed(void *p);
void Player_SetDirectionalVelocity(void *, s32);
void Player_PlayPositionEffect(void *, void *, u32, u32, void *, u32, u32);

static inline u8 global_bit(void)
{
    u32 mask = 1;
    if (*(u32 *)((u8 *)gGameState + 0x868) & mask)
        return TRUE;
    return FALSE;
}

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

void Player_UpdateWalkFootsteps(u8 *p)
{
    struct DisplaySequenceState *seq;

    if (Player_ConsumeStateEntry(p))
        p[0x46a] = 0;
    *(u32 *)(p + 0x20) &= ~1;
    p[0x3a4] = Player_GetSmoothedDirection(p);
    Player_SetDirectionalVelocity(p, Player_GetWalkSpeed(p));
    Player_UpdateFacingSide(p);
    seq = (struct DisplaySequenceState *)(p + 0x3fc);
    Anim_SetSequence(seq, *(void **)(p + 0x410), 1, p[0x3a2], p[0x3a3]);
    Step((struct ContactSwarmSprite *)(p + 0x14C), (struct DisplaySequenceState *)(p + 0x3FC));
    {
        register u32 offset asm("r2") = 0x404;
        register u16 *frame asm("r0");
        asm("" : "+r"(offset));
        frame = (u16 *)(p + offset);
        if (*frame == 2) {
            u32 z = *(u16 *)(p + 0x40a);
            u8 *fx;
            if (z == 0) {
                fx = p + 0x30;
                asm("" : "+r"(fx));
                if (RepeatedEffect_RequestNearby(fx)) {
                    if (p[0x46a]) {
                        p[0x46a] = z;
                        Player_PlayPositionEffect(p, fx, 1, 1, fx, z, 0x163);
                    } else {
                        p[0x46a] = 1;
                        Player_PlayPositionEffect(p, fx, 1, 1, fx, 0, 0x162);
                    }
                } else {
                    if (p[0x46a]) {
                        p[0x46a] = 0;
                        Sound_PlaySong(0xc7);
                    } else {
                        p[0x46a] = 1;
                        Sound_PlaySong(0xc8);
                    }
                    if (global_bit())
                        Player_PlayPositionEffect(p, fx, 1, 2, 0, 0, 0);
                }
            }
        }
    }
}
