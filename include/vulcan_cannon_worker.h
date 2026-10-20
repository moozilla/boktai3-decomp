#ifndef GUARD_VULCAN_CANNON_WORKER_H
#define GUARD_VULCAN_CANNON_WORKER_H
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
/* Layout observed in initialization, state callbacks and collision consumers. */
struct VulcanCannon {
    u8 pad0[0x18];
    s32 timer;
    u8 body[0x48];
    struct SceneVec initialPosition;
    union {
        struct DisplaySequenceSprite sprite;
        struct {
            u8 pad[0x1c];
            struct SceneVec position;
        } spatial;
    } display;
    u8 graphics[0x1c];
    u8 collider[0x54];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s32 range;
    s16 attack, element, power, hp, damage, damaged, unknown12c, dead, flash;
    s8 facing;
    u8 allowTurn;
    s16 turnDelay, speed, shotCount, targetVisible, previousVisible;
    s16 burstDelay, burstCount, aimTimer, maxHp, bodyFlags, bodyPower;
    s16 hudStyle, unknown14c, hudDuration, defeatId, unknown152, inert;
    u16 unknown156;
    void *owner;
    u32 hitFlags, colliderHitFlags, args[3];
    void *delayScript;
    u32 deathScript, terrainEnabled;
    void *resource, *attachment;
    u8 terrain[0x10];
    s16 delayCount, delayIndex, delays[8];
};
#ifndef VC_FIRST
#define VC_FIRST    "r1"
#define VC_TEMP     "r1"
#define VC_MASK     "r5"
#define VC_DURATION "r3"
#define VC_WEIGHT   "r1"
#endif
static inline u32 VulcanCannon_StepAnimation(struct VulcanCannon *owner)
{
    struct DisplaySequenceSprite *sprite = &owner->display.sprite;
    struct DisplaySequenceState *state = &owner->sequence;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm(VC_FIRST);
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
        register u32 first asm(VC_FIRST);
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
        register u32 mask asm(VC_MASK);
        register u32 temp asm(VC_TEMP);
        register u32 savedDuration asm(VC_DURATION);
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
                register s32 weight asm(VC_WEIGHT) = state->frameWeight;
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
    return ended;
}

#endif
