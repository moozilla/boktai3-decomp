// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
static inline void Step(struct SceneSteeringActor *slot, struct DisplaySequenceState *state)
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

struct SceneSteeringActorPool *SceneSteeringActor_EnsurePool(void);
struct SceneSteeringActor *SceneSteeringActor_Allocate(struct SceneSteeringActorPool *);
void SceneSteeringActor_InitializeCollider(struct SceneSteeringActor *, s32, s32, s32, s32, s32,
                                           s32, s32);
u32 ScaledShadow_Initialize(void *, const struct ContactPosition *, s32, s32, s32, s32, s32, s32,
                            s32, s32, s32);
void ScaledShadow_SetGraphic(void *, s32, s32);
void ScaledShadow_SetFlag(void *, s32);
void ScaledShadow_Disable(void *);
void SceneSteeringActor_CreateTravelActor(const struct ContactPosition *position,
                                          const struct ContactPosition *target, s32 velocityY,
                                          s32 direction, s32 steeringTimer, s32 turnStep,
                                          s32 steeringDelay, s32 speed, s32 speedLimit, s32 a,
                                          s32 b, s32 c, s32 d)
{
    const struct ContactPosition *savedPosition = position;
    register const struct ContactPosition *savedTarget asm("r6") = target;
    register s32 savedVelocityY asm("r10") = velocityY;
    register s32 savedDirection asm("r4") = direction;
    struct SceneSteeringActorPool *pool;
    struct SceneSteeringActor *slot;
    register u32 delayOffset asm("r9");
    register struct SceneSteeringActor *sprite asm("r10");
    struct DisplaySequenceState *sequence;
    const void **resourceSlot;
    u32 clear;
    pool = gSceneSteeringActorPool;
    if (!pool) {
        pool = SceneSteeringActor_EnsurePool();
        if (!pool)
            return;
    }
    slot = SceneSteeringActor_Allocate(pool);
    if (!slot)
        return;
    {
        u8 *field = &slot->state;
        register u32 value asm("r2");
        value = 1;
        *field = value;
    }
    {
        register u32 offset asm("r0") = 0x11c;
        register s32 *field asm("r1");
        asm("" : "+r"(offset));
        field = (void *)((u8 *)slot + offset);
        *field = 120;
    }
    {
        u32 offset = 0x120;
        register u32 zeroValue asm("r2");
        register u16 *field asm("r0");
        field = (void *)((u8 *)slot + offset);
        zeroValue = 0;
        *field = zeroValue;
        offset -= 0x14;
        field = (void *)((u8 *)slot + offset);
        *(u8 *)field = savedDirection;
    }

    slot->target = *savedTarget;
    slot->direction = savedDirection;
    slot->steeringTimer = steeringTimer;
    slot->turnStep = turnStep;
    {
        register u32 offset asm("r1") = 0x127;
        delayOffset = offset;
        *((u8 *)slot + offset) = steeringDelay;
    }
    slot->speed = speed;
    slot->parameter = speedLimit;
    {
        u8 *field = &slot->enter;
        register u32 value asm("r2");
        value = 1;
        *field = value;
    }
    {
        struct ContactPosition *destination = &slot->velocity;
        register u32 clearValue asm("r0");
        clearValue = 0;
        clear = clearValue;
        {
            u32 *source = &clear;
            u32 mode;
            asm("" : "+r"(source));
            mode = 0x05000002;
            CpuSet(source, destination, mode);
        }
    }
    {
        s16 *field = &slot->velocity.y;
        register s32 y asm("r1");
        y = savedVelocityY;
        *field = y;
    }
    sprite = slot;
    {
        const struct ContactPosition *source = savedPosition;
        slot->position = *source;
    }
    {
        const void *resource = ScriptProc_Start(0x922E, 0x121B);
        u32 offset = 0x118;
        resourceSlot = (void *)((u8 *)slot + offset);
        *resourceSlot = resource;
    }
    Sprite_Init(slot, pool->graphics, 0);
    sequence = &slot->sequence;
    {
        const void *resource = *resourceSlot;
        u32 animationFlags;
        animationFlags = 0;
        asm("" : "+r"(animationFlags) : "r"(resource));
        Anim_SetSequence(sequence, (void *)resource, 7, 0, animationFlags);
    }
    /* The original reuses the steering-delay field offset as the palette
     * argument; preserve the observed value 0x127 rather than guessing a bank. */
    AnimData_SelectPaletteBank(slot->graphics, delayOffset);
    Step(slot, sequence);
    {
        u32 tailZero = 0;
        u32 priority = 2;
        struct SceneSteeringActor *owner;
        void *shadow;
        owner = sprite;
        owner->priority = priority;
        owner->flags &= ~1;
        SceneSteeringActor_InitializeCollider(slot, a, b, c, d, tailZero, tailZero, tailZero);
        shadow = slot->shadow;
        slot->shadowStatus = ScaledShadow_Initialize(shadow, &slot->position, 64, 64, 64, 100,
                                                     tailZero, tailZero, 5, 1, tailZero);
        ScaledShadow_SetGraphic(shadow, 0x1C1B, 6);
        ScaledShadow_SetFlag(shadow, 0);
        ScaledShadow_Disable(shadow);
    }
}
