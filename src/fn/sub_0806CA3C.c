// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
static inline void Step(struct SceneSteeringActor *slot, struct DisplaySequenceState *state,
                        u32 one)
{
    register const volatile u16 *command asm("r4");
    u32 ended, bit;
    u32 duration;
    register u32 flags asm("r1");
    {
        register u32 offset asm("r0") = state->index << 1;
        register const volatile u16 *base asm("r1") = slot->sequence.commands;
        command = (const volatile u16 *)((u32)base + offset);
    }
    slot->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = one;
        bit = maskBit;
        asm("" : "+r"(bit));

        first = bit;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            flip &= bits;
            flip >>= 4;
            flip &= bit;

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
                register u32 offset asm("r0") = state->index << 1;
                register const volatile u16 *table asm("r1");
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
void SceneSteeringActor_CreateBobbingActor(const struct ContactPosition *position,
                                           const struct ContactPosition *target, s32 velocityY,
                                           s32 direction, s32 steeringTimer, s32 turnStep,
                                           s32 steeringDelay, s32 speed, s32 speedLimit, s32 a,
                                           s32 b, s32 c, s32 d)
{
    const struct ContactPosition *savedTarget = target;
    s32 savedVelocityY = velocityY;
    s32 savedDirection = direction;
    struct SceneSteeringActorPool *pool;
    struct SceneSteeringActor *slot;
    register u32 zero asm("r8");
    u32 one;
    struct SceneSteeringActor *sprite;
    register struct DisplaySequenceState *sequence asm("r5");
    u32 paletteToken;
    u32 clear;
    void *shadow;
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
        u32 offset = 0x122;
        u8 *state = (void *)((u8 *)slot + offset);
        u32 temp = 0;
        zero = temp;
        *state = 2;
        asm("" : "+r"(zero));
    }
    slot->lifetime = 120;
    {
        u32 offset = 0x120;
        register u16 *flags asm("r0") = (void *)((u8 *)slot + offset);
        u32 temp = zero;
        *flags = temp;
        offset -= 0x14;
        {
            register u8 *field asm("r0") = (void *)((u8 *)slot + offset);
            *field = savedDirection;
        }
    }
    {
        u32 offset = 0x104;
        struct ContactPosition *field;
        field = (void *)((u8 *)slot + offset);
        *field = *savedTarget;
    }
    {
        u32 offset = 0x10D;
        {
            u8 *field = (void *)((u8 *)slot + offset);
            *field = savedDirection;
        }
    }
    {
        u32 offset = 0x10E;
        *(u8 *)((u8 *)slot + offset) = steeringTimer;
        {
            u32 turnOffset = 0x10F;
            *(u8 *)((u8 *)slot + turnOffset) = turnStep;
        }

        {
            register u32 offset asm("r1") = 0x127;
            paletteToken = offset;
            *(u8 *)((u8 *)slot + offset) = steeringDelay;
        }
        offset += 2;
        {
            register u16 *field asm("r1") = (void *)((u8 *)slot + offset);
            *field = speed;
        }
    }
    {
        register u32 offset asm("r0") = 0x112;
        asm("" : "+r"(offset));
        {
            u16 *field = (void *)((u8 *)slot + offset);
            register s32 value asm("r0") = speedLimit;
            *field = value;
        }
    }
    {
        u32 offset = 0x126;
        u8 *field = (void *)((u8 *)slot + offset);
        one = 1;
        *field = one;
    }
    {
        register struct ContactPosition *destination asm("r1") = &slot->velocity;
        clear = zero;
        {
            register u32 *source asm("r0") = &clear;
            CpuSet(source, destination, 0x05000002);
        }
    }
    slot->velocity.y = savedVelocityY;
    sprite = slot;
    slot->position = *position;
    slot->sequenceResource = ScriptProc_Start(0x922E, 0x121B);
    Sprite_Init(slot, pool->graphics, 0);
    sequence = &slot->sequence;
    Anim_SetSequence(sequence, (void *)slot->sequenceResource, 7, 0, zero);
    AnimData_SelectPaletteBank(slot->graphics, paletteToken);
    Step(slot, sequence, one);
    {
        u32 finalZero = 0;
        u32 priority = 2;
        register struct SceneSteeringActor *late asm("r2");
        late = sprite;
        late->priority = priority;
        late->flags &= ~1;
        SceneSteeringActor_InitializeCollider(slot, a, b, c, d, finalZero, finalZero, finalZero);
        shadow = slot->shadow;
        slot->shadowStatus = ScaledShadow_Initialize(shadow, &slot->position, 64, 64, 64, 100,
                                                     finalZero, finalZero, 5, 1, finalZero);
        ScaledShadow_SetGraphic(shadow, 0x1C1B, 6);
        ScaledShadow_SetFlag(shadow, 0);
        ScaledShadow_Disable(shadow);
    }
}
