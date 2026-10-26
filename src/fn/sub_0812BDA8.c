// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "engine.h"
#include "ram.h"
struct Slash {
    u32 flags;
    u8 active, unknown5, phase, priority;
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 trails[0xB8];
    u8 primary[0x54], secondary[0x54];
    struct ContactPosition velocity;
    u8 initialHeading, heading, turnTimer, turnSpeed;
    u16 speed;
    u8 trailIndex, trailPeriod;
    const void *sequenceResource;
    s32 lifetime;
    u16 effectFlags;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered, turnDelay;
};
struct Pool {
    u8 task[0x18];
    u8 graphics[0x20];
    u32 used;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
struct Pool *SlashEffect_GetOrCreateManager(void);
struct Slash *SlashEffect_AllocateSlot(struct Pool *);
void TwinSpriteEffect_InitGraphics(struct Slash *);
void PursuingActor_InitColliders(struct Slash *, s32, s32, s32, s32, s32, s32, s32);
void TimedEffect_UpdateVisibility(struct Slash *);
void TwinSpriteEffect_EmitTrail(struct Slash *);
void PursuingActor_TurnTowardPlayer(struct Slash *);
void SlashEffect_UpdateColliders(struct Slash *);
void SwooshProjectile_RemoveTrailSprites(struct Slash *);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void Step(struct Slash *slot, struct DisplaySequenceState *state)
{
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table = slot->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    slot->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        first = 1;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            u32 flipBit;
            flip &= bits;
            flip >>= 4;
            flipBit = 1;
            flip &= flipBit;

            if (first != flip)
                slot->flags |= 4;
            else
                slot->flags &= ~4;
        }
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
            slot->flags |= 8;
        else
            slot->flags &= ~8;
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
        asm("" : "+&r"(mask) : "r"(temp));
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
        } else {
        }
    }
    return;
}

// confidence=low: paired-collider slash resource 8DC7, with a homing heading and trail sprites
void PursuingActor_CreateHomingSlash(const struct ContactPosition *position, s32 heading,
                                     s32 turnTimer, s32 turnSpeed, s32 turnDelay, s32 speed,
                                     s32 phase, s32 a, s32 b, s32 c, s32 d)
{
    const struct ContactPosition *savedPosition = position;
    s32 savedHeading = heading;
    register s32 savedTurnTimer asm("r6") = turnTimer;
    register s32 savedTurnSpeed asm("r9") = turnSpeed;
    struct Pool *pool;
    struct Slash *slot;
    register struct Slash *sprite asm("r9");
    register struct DisplaySequenceState *sequence asm("r5");
    const void **resourceSlot;
    register u32 zero asm("r8");
    pool = gPursuingActorManager;
    if (!pool) {
        pool = SlashEffect_GetOrCreateManager();
        if (!pool)
            return;
    }
    slot = SlashEffect_AllocateSlot(pool);
    if (!slot)
        return;
    {
        u32 offset = 0x1B6;
        register u8 *state asm("r0") = (u8 *)slot + offset;
        register u32 clear asm("r2") = 0;
        zero = clear;
        asm("" : "+r"(zero) : "r"(state), "r"(clear));
        *state = 1;
    }
    {
        register u32 offset asm("r2") = 0x1B0;
        asm("" : "+r"(offset));
        {
            register s32 *field asm("r1") = (void *)((u8 *)slot + offset);
            *field = 120;
        }
    }
    {
        u32 even = 0x1B4;
        register u16 *field asm("r0") = (void *)((u8 *)slot + even);
        u32 clear = zero;
        asm("" : "+r"(clear));
        *field = clear;
        even -= 0x10;
        *(u8 *)((u8 *)slot + even) = savedHeading;
        {
            register u32 odd asm("r2") = 0x1A5;
            *(u8 *)((u8 *)slot + odd) = savedHeading;
            even += 2;
            {
                register u8 *field asm("r0") = (u8 *)slot + even;
                *field = savedTurnTimer;
            }
            odd += 2;
            *(u8 *)((u8 *)slot + odd) = savedTurnSpeed;
            odd += 0x14;
            {
                register u8 *field asm("r1") = (u8 *)slot + odd;
                *field = turnDelay;
            }
        }
    }
    slot->speed = speed;
    {
        u8 *field = &slot->entered;
        register u32 one asm("r2") = 1;
        *field = one;
    }
    sprite = slot;
    {
        register const struct ContactPosition *source asm("r2") = savedPosition;
        asm("" : "+r"(source));
        slot->position = *source;
    }
    {
        const void *resource = ScriptProc_Start(0x922E, 0x8DC7);
        resourceSlot = &slot->sequenceResource;
        *resourceSlot = resource;
    }
    Sprite_Init(slot, pool->graphics, 0);
    sequence = &slot->sequence;
    {
        const void *resource = *resourceSlot;
        register u32 clear asm("r2") = zero;
        asm("" : "+r"(clear));
        Anim_SetSequence(sequence, resource, 1, 0, clear);
    }
    AnimData_SelectPaletteBank(slot->graphics, 0x2E2);
    Step(slot, sequence);
    {
        u32 finalZero = 0;
        u32 priority = 1;
        struct Slash *late = sprite;
        late->priority = priority;
        late->flags = (late->flags | 2) & ~1;
        late->phase = phase;
        TwinSpriteEffect_InitGraphics(slot);
        PursuingActor_InitColliders(slot, a, b, c, d, finalZero, finalZero, finalZero);
    }
}
