// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "bouncing_object.h"
#include "engine.h"
static inline u32 Blocked(u32 mask)
{
    u32 a = gEngineFlags;
    u32 b = gScenePauseFlags;
    return (a | b) & mask;
}
static inline void SetFlag(u32 *flags, u32 mask)
{
    u32 value = *flags;
    register u32 m asm("r3") = mask;
    value |= m;
    *flags = value;
}
struct BouncingObject *BouncingObject_Spawn(struct ContactPosition *source,
                                            struct ContactPosition *inDelta,
                                            struct ContactPosition *inTarget, u32 inShot,
                                            u32 category, u32 strength, u32 id)
{
    register struct ContactPosition *delta asm("r8") = inDelta;
    register struct ContactPosition *target asm("r4") = inTarget;
    u32 shot = inShot;
    struct BouncingPool *pool;
    pool = gBouncingPool;
    if (!pool) {
        pool = BouncingObject_EnsurePool();
        if (!pool)
            return NULL;
    }
    if (pool->stopping)
        return NULL;
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return NULL;
    }
    {
        u32 zero;
        struct BouncingObject *object;
        struct BouncingObject *sprite;
        u32 one;
        {
            zero = gUnk_02000560;
            {
                register u32 blockedMask asm("r2") = 4;
                zero &= blockedMask;
            }
        }
        if (zero)
            return NULL;
        object = BouncingObject_Allocate(pool);
        if (!object)
            return NULL;
        {
            u32 off = 0x136;
            u32 base = (u32)object;
            {
                u8 *q = (u8 *)(base + off);
                register u32 d asm("r2");
                one = 1;
                asm("" ::"r"(d));
                *q = one;
            }
            off -= 2;
            *(u16 *)(base + off) = zero;
            object->origin = *target;
            object->enter = one;
            off -= 0x10;
            {
                struct ContactPosition *dst = (void *)(base + off);
                register struct ContactPosition *src asm("r3") = delta;
                *dst = *src;
            }
            object->spawnParameterA = shot;
            off = 0x140;
            *(u16 *)(base + off) = category;
            object->initialTimer = (u16)strength;
            off -= 6;
            *(u16 *)(base + off) = (u16)strength;
            object->collisionDelay = 30;
            off += 0xa;
            {
                u32 *dst = (void *)(base + off);
                *dst = id;
            }
            object->animation = one;
        }
        sprite = object;
        {
            register struct ContactPosition *src asm("r3") = source;
            asm("" : "+r"(src));
            object->sprite.position = *src;
        }
        Sprite_Init(object, pool->graphics, 0);
        Anim_SetSequence(&object->sequence, pool->sequenceResource, 1, zero, 0);
        asm("" : "+r"(one));
        {

            struct DisplaySequenceState *state = &object->sequence;
            const volatile u16 *command =
                (const volatile u16 *)((u32)state->commands + (state->index << 1));
            u32 ended, duration, flags, bit;
            object->sprite.frame = *command >> 6;
            {
                u32 input = state->flags;
                register u32 first asm("r1");
                bit = one;
                first = bit;
                first &= input;
                {
                    u32 bits = *command;
                    u32 flip = 0x30;
                    flip &= bits;
                    flip >>= 4;
                    flip &= one;
                    if (first != flip)
                        SetFlag(&object->sprite.flags, 4);
                    else
                        object->sprite.flags &= ~4;
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
                if (first != (((*command & 0x30) >> 4) & bit)) {
                    struct BouncingObject *read = sprite;
                    u32 value;
                    value = read->sprite.flags;
                    value |= 8;
                    {
                        struct BouncingObject *write = sprite;
                        write->sprite.flags = value;
                    }
                } else {
                    register struct BouncingObject *p asm("r3") = sprite;
                    p->sprite.flags &= ~8;
                }
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
        }
        {
            register struct BouncingObject *w asm("r2") = sprite;
            w->sprite.flags |= 1;
            w->sprite.priority = 1;
        }
        Shadow_Initialize(object->registration, &sprite->sprite.position, 0);
        BouncingObject_InitializeColliders(object, 0, 0);
        return object;
    }
}
