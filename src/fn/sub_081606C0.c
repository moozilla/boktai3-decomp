#include "global.h"

// MATCHING_CONCESSION: register-pinning
// confidence=low: allocates one of sixteen attack particle slots and initializes its animation.
#define PlayerAttackEffect_AllocateParticle sub_081606C0
#include "contact_swarm.h"
struct Vec {
    s16 x, y, z, pad;
};
s32 Math_Atan2(s32, s32);
void Sprite_ApplyAnimation(void *, void *);
void Anim_RestartSequence(void *, void *, u16, u32, u32);
inline static u32 Step_1(struct ContactSwarmSprite *sprite, struct ContactSwarmSprite *draw,
                         struct DisplaySequenceState *frames, u32 one)
{
    struct DisplaySequenceState *state = frames;
    register u32 maskBit asm("r9") = one;
    const volatile u16 *command;
    u32 ended;
    u32 bit;
    u32 duration;
    u32 flags;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        bit = maskBit;
        first = maskBit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & maskBit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        temp = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = temp;
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

s32 PlayerAttackEffect_AllocateParticle(u8 *arg, struct Vec *pos, s32 lifetime, void *anim,
                                        s32 sequence, s32 kind)
{
    u8 *p = arg;
    struct Vec *position = pos;
    s32 remaining = lifetime;
    void *animation = anim;
    register s32 mode asm("r9") = kind;
    s32 i;
    register s32 increment asm("r3");
    s32 check;
    u32 *slot = (u32 *)(p + 0x484);
    struct DisplaySequenceState *state;
    i = 0;
    state = (void *)(p + 0x384);
    do {
        if (slot[0] == 0) {
            s32 offset = 0x2c * i;
            struct ContactSwarmSprite *sprite = (void *)((offset + (u32)p) + 0xc4);
            s32 angle;
            u8 *positions;
            sprite->flags &= ~1;
            positions = p + 0xe0;
            positions += offset;
            *((struct Vec *)positions) = *position;
            slot[0] = remaining;
            Sprite_ApplyAnimation((void *)(((0x2c * i) + 0xc4) + p), animation);
            slot[1] = sequence;
            slot[2] = mode;
            angle = Math_Atan2(position->x - (*((s16 *)(*((u8 **)(p + 0x594)) + 0x20))),
                               position->z - (*((s16 *)(*((u8 **)(p + 0x594)) + 0x24))));
            slot[3] = angle;
            if (*(*((u8 **)(p + 0x590))))
                slot[3] = angle + 64;
            else
                slot[3] = angle - 64;
            slot[3] = *((u8 *)(slot + 3));
            {
                register s32 modeCheck asm("r3") = mode;
                asm("" : "+r"(modeCheck));
                if (modeCheck == 1) {
                    register u32 resourceOffset asm("r1") = 0x5a4;
                    register void **resourceSlot asm("r0");
                    void *resource;
                    asm("" : "+r"(resourceOffset));
                    resourceSlot = (void **)(p + resourceOffset);
                    resource = *resourceSlot;
                    Anim_RestartSequence(state, resource, (u16)sequence, 0, 0);
                    Step_1((void *)(((0x2c * i) + p) + 0xc4), (void *)(((0x2c * i) + p) + 0xc4),
                           state, mode);
                }
            }
            return 1;
        }
        state++;
        increment = 1;
        asm("" : "+r"(increment));
        i += increment;
        slot += 4;
        check = i;
        asm("" : "+r"(check));
    } while (check < 16);

    return 0;
}
