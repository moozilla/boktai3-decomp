#include "global.h"
#include "display_animation.h"
#ifdef NONMATCHING
// fuzzy: 99.1%
// fix.py best after the inlined-insn fix
// (this draft uses local register pinning)
// Remaining: the ROM stores paletteData off p (`str r0, [r6, #0x50]`); writing
// it as p->paletteData raises p's allocno priority and moves p to r5, so the
// draft stores it off `resource`. The owner copy for the store uses r1, not r2.
struct WheelSprite {
    u32 flags;
    u8 pad4[0xC];
    u16 frame;
    u8 pad12[0xA];
    s16 x, y, z;
    u8 pad22[0xA];
};
struct Wheel {
    u8 task[0x18];
    struct WheelSprite sprite;
    u8 resource[6];
    u16 paletteIndex;
    u8 resource8[4];
    void *paletteData;
    u8 resource10[0xC];
    struct DisplaySequenceState animation;
    void *animationData;
    u8 receiver[0x34];
    void *owner;
    u8 palette[0x18];
    u8 paletteBuffer[0x20];
};
extern u8 *gUnk_030042E4;
void TimePalette_InitTransition(void *, void *, void *, void *);
void SceneMessages_RegisterReceiver(void *, void *, s32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
static inline u32 StepPlayback(struct Wheel *owner, struct DisplaySequenceState *state,
                               struct Wheel *base, struct WheelSprite *sprite)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->animation.commands;
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
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
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
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
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
    return ended;
}
inline static u32 StepPlayback_081C49F4(struct Wheel *owner, struct DisplaySequenceState *state,
                                        struct Wheel *base, struct WheelSprite *sprite)
{
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->animation.commands;
        do {
            command = (const volatile u16 *)(((u32)table) + offset);
        } while (0);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
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
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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

inline static u32 Arg1(u32 k_, u8 *resource)
{
    return resource + k_;
}

s32 sub_081C49F4(struct Wheel *p, void *owner)
{
    struct WheelSprite *sprite = &p->sprite;
    u8 *resource = p->resource;
    AnimData_Load(resource, 0x2173);
    TimePalette_InitTransition(p->palette, gUnk_030042E4 + 0x2D20, gUnk_030042E4 + 0x2D40,
                               gUnk_030042E4 + 0x2D60);
    *((void **)Arg1(0xC, resource)) = p->paletteBuffer;
    p->paletteIndex = 0x169;
    Sprite_Init(sprite, resource, 0);
    p->animationData = ScriptProc_Start(0x922E, 0x2990);
    if (!p->animationData)
        return -1;
    Anim_SetSequence(&p->animation, p->animationData, 0, 0, 0);
    StepPlayback_081C49F4(p, &p->animation, p, sprite);
    if (Script_SeekToKeyword('p')) {
        sprite->x = Script_GetValue();
        sprite->y = Script_GetValue();
        sprite->z = Script_GetValue();
        {
            void **slot = &p->owner;
            *slot = owner;
            asm("" : : : "memory");
            SceneMessages_RegisterReceiver(((u8 *)slot) - 0x34, owner, 10);
        }
        return 0;
    }
    return -1;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081C49F4);
#endif
