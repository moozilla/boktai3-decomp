// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player.h"
// confidence=low: random neighboring directions with short hold durations.
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: RNG allocation, even-direction address scheduling and literal pools.
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

u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
struct ReleaseAnimation {
    u8 pad[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad2[0x3FC - 0x14C - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 Player_GetSmoothedDirection(struct Player *);
void Player_SetDirectionalVelocity(struct Player *, s32);
static inline s32 RandomBits(u32 mask)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return gUnk_0203B400[index];
}
// confidence=low: selects nearby directions with randomized hold durations.
static inline void Step_UpdateWaveringMovement(struct ContactSwarmSprite *sp,
                                               struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
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

void Player_UpdateWaveringMovement(struct Player *p)
{
    u8 *timer;
    if (Player_ConsumeStateEntry(p))
        *(((u8 *)p) + 0x469) = 0;
    p->statusFlags &= ~1;
    timer = ((u8 *)p) + 0x469;
    if ((*timer) == 0) {
        s32 direction = Player_GetSmoothedDirection(p);
        s16 choice;
        if (direction & 1) {
            if (direction > 4) {
                p->heading = (8 - direction) >> 1;
                p->side = 1;
            } else {
                p->heading = direction >> 1;
                p->side = 0;
            }
            choice = RandomBits(7) & 7;
            if (choice <= 5) {
                p->facing = direction;
                p->unk466[3] = 9;
            } else if (choice <= 6) {
                p->facing = (direction + 7) & 7;
                p->unk466[3] = 3;
            } else {
                p->facing = (direction + 9) & 7;
                p->unk466[3] = 3;
            }
        } else {
            choice = RandomBits(7) & 7;
            if (choice <= 5) {
                p->facing = direction;
                *timer = 9;
            } else {
                if (choice <= 6)
                    p->facing = (direction + 7) & 7;
                else
                    p->facing = (direction + 9) & 7;
                *timer = 3;
            }
            if (!(p->facing & 1))
                direction = (p->facing + (((RandomBits(1) & 1) << 1) + 7)) & 7;
            else
                direction = p->facing;
            if (direction > 4) {
                p->heading = (8 - direction) >> 1;
                p->side = 1;
            } else {
                {
                    typeof(p->heading) *q1 = &p->heading;
                    u8 z1 = 0;
                    *q1 = direction >> 1;
                    p->side = z1;
                }
            }
        }
    } else
        --(*timer);
    Player_SetDirectionalVelocity(p, 9);
    Anim_SetSequence(&((struct ReleaseAnimation *)p)->seq, *((const void **)(((u8 *)p) + 0x40C)), 0,
                     p->heading, p->side);
    Step_UpdateWaveringMovement(&((struct ReleaseAnimation *)p)->sprite,
                                &((struct ReleaseAnimation *)p)->seq);
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateWaveringMovement);
#endif
