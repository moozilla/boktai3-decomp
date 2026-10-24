// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"

struct CannonPaletteColor {
    u16 value;
} __attribute__((packed, aligned(2)));

struct CannonEmitter {
    u8 pad0[0x18];
    struct SceneVec initialPosition;
    struct DisplaySequenceSprite sprite;
    u8 animationData[0x1c];
    u8 body[0x48];
    struct CannonPaletteColor flashPalette[16];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s32 rangeSquared;
    s16 effectX, effectY, effectZ, solid;
    s16 timer, flashTimer, variant, spread;
    void *parent;
    u8 padFC[4];
    void *resource;
};
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
s32 TerrainOverlay_QueueAddActor(void *);
void ProximitySequence_TestPlayerDistance(void);
static inline u32 CannonEmitter_StepAnimation(struct CannonEmitter *owner,
                                              struct DisplaySequenceState *stateArg, u32 flipMask)
{
    register struct DisplaySequenceSprite *sprite asm("ip") = &owner->sprite;
    struct DisplaySequenceState *state = stateArg;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    {
        u32 frame = *command >> 6;
        {
            register struct DisplaySequenceSprite *out asm("r3") = &owner->sprite;
            asm("" : "+r"(out));
            out->frame = frame;
        }
    }
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        register u32 maskBit asm("r4") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit)) {
            u32 flags = owner->sprite.flags;
            register u32 flip = flipMask;
            owner->sprite.flags = flags | flip;
        } else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r4");
        register u32 first asm("r2");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        {
            register u32 flags asm("r1");
            if (first != (((*command & 0x30) >> 4) & bit)) {
                register struct DisplaySequenceSprite *read asm("r2") = sprite;
                flags = read->flags | 8;
            } else {
                register struct DisplaySequenceSprite *read asm("r2") = sprite;
                flags = read->flags & ~8;
            }
            {
                register struct DisplaySequenceSprite *out asm("r3") = sprite;
                out->flags = flags;
            }
        }
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r4");
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
                register s32 weight asm("r2") = state->frameWeight;
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

s32 CannonEmitter_InitializeFromScript(struct CannonEmitter *p, void *parent)
{
    register s32 result asm("r0");
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 range = Script_GetValue();
        p->rangeSquared = range * range;
    } else
        p->rangeSquared = 0x40000;
    if (Script_SeekToKeyword('a')) {
        p->effectX = Script_GetValue();
        p->effectY = Script_GetValue();
        p->effectZ = Script_GetValue();
    } else {
        p->effectX = 0;
        p->effectY = 0;
        p->effectZ = 30;
    }
    if (Script_SeekToKeyword('t'))
        p->variant = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        p->spread = Script_GetValue();
    else
        p->spread = 2;
    if (Script_SeekToKeyword('S'))
        p->solid = Script_GetValue();
    p->parent = parent;
    AnimData_Load(p->animationData, 0x7e7d);
    Sprite_Init(&p->sprite, p->animationData, 1);
    *(struct SceneVec *)((u8 *)p + 0x3c) = p->initialPosition;
    AnimData_SelectPaletteBank(p->animationData, p->variant + 0x1ba);
    {
        s32 i = 15;
        struct CannonPaletteColor *palette;
        s32 first;
        do {
            first = (s32)p->flashPalette;
            palette = &p->flashPalette[i];
            palette->value = 0x7fff;
            --i;
        } while ((s32)palette - (s32)sizeof(*palette) >= first);
    }
    p->resource = ScriptProc_Start(0x922e, 0x5184);
    result = Anim_SetSequence(&p->sequence, p->resource, 6, 0, 0);
    CannonEmitter_StepAnimation(p, &p->sequence, 4);
    if (p->solid) {
        SceneBody_Init(p->body, p->parent, (u8 *)p + 0x3c, 0, 4, p);
        SceneBody_SetSprite(p->body, &p->sprite);
        result = TerrainOverlay_QueueAddActor(p->body);
    }
    p->state = ProximitySequence_TestPlayerDistance;
    return result;
}
