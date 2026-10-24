// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "contact_swarm.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
static inline u32 StepPlayback(struct Drawable *owner, struct DisplaySequenceState *state, u32 one)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
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
        register u32 first asm("r1");
        u32 maskBit = one;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
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
    return ended;
}

void SingletonSlot7_InitDisplaySprites(u8 *p)
{
    struct Drawable *foreground = (struct Drawable *)(p + 0x2C0);
    struct Drawable *accent = (struct Drawable *)(p + 0x308);
    u8 *animA = p + 0x350;
    struct DisplaySequenceState *state;
    u32 zero, one, z;
    AnimData_Load(animA, *(u32 *)(p + 0x388));
    AnimData_SelectPaletteBank(animA, 3);
    {
        u8 *palA = p + 0x35C;
        CpuSet(*(void **)palA, p + 0x3C8, 0x10);
        *(void **)palA = p + 0x3C8;
    }
    Sprite_Init(foreground, animA, 0x11);
    z = 0;
    one = 1;
    asm("" : "+r"(one));
    foreground->sprite.priority = one;
    foreground->unknown3C = z;
    state = (struct DisplaySequenceState *)(p + 0x2EC);
    Anim_SetSequence(state, *(void **)(p + 0x390), 0, 0, 0);
    StepPlayback(foreground, state, one);
    {
        register u32 d asm("r1");
        zero = 0;
        foreground->sprite.position.x = zero;
        foreground->sprite.position.y = zero;
        foreground->sprite.position.z = zero;
        asm("" ::"r"(d));
    }
    {
        u8 *animB = p + 0x36C;
        AnimData_Load(animB, *(u32 *)(p + 0x388));
        AnimData_SelectPaletteBank(animB, 2);
        {
            u8 *palB = p + 0x378;
            CpuSet(*(void **)palB, p + 0x3E8, 0x10);
            *(void **)palB = p + 0x3E8;
        }
        Sprite_Init(accent, animB, 0x11);
        one = 1;
        asm("" : "+r"(one));
        accent->sprite.priority = one;
        accent->unknown3C = 4;
        state = &accent->sequence;
        Anim_SetSequence(state, *(void **)(p + 0x390), 4, 0, zero);
        StepPlayback(accent, state, one);
        {
            register u32 d asm("r1");
            accent->sprite.position.x = 0;
            accent->sprite.position.y = 0;
            accent->sprite.position.z = 0;
            asm("" ::"r"(d));
        }
    }
}
