// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
static inline void stepSecondarySprite(struct SceneRigSprite *sprite, u32 byteMask)
{
    struct DisplaySequenceState *state = &sprite->sequence;
    const u16 *command;
    {
        u32 offset = state->index << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        {
            register u32 flags asm("r0");
            struct SceneRigSprite *out;
            if (first != (((*command & 0x30) >> 4) & 1)) {
                out = sprite;
                asm("" : "+r"(out));
                flags = out->flags | 4;
            } else {
                flags = sprite->flags & ~4;
                out = sprite;
            }
            out->flags = flags;
        }
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit)) {
            register struct SceneRigSprite *out asm("r5") = sprite;
            out->flags |= 8;
        } else {
            register struct SceneRigSprite *read asm("r1") = sprite;
            u32 flags;
            struct SceneRigSprite *out;
            asm("" : "+r"(read));
            flags = read->flags & ~8;
            out = sprite;
            out->flags = flags;
        }
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        {
            s32 delay = (state->frameWeight * (s32)state->speed) >> 6;
            state->duration = delay;
            if (!(delay & byteMask))
                state->duration = 1;
        }
    }
}

static inline s32 scale12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline s32 scale3(s32 v)
{
    return v >= 0 ? v >> 3 : -((-v) >> 3);
}
static inline s32 scale5(s32 v)
{
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}
void ScenePanelRig_TickSecondarySprites(struct ScenePanelRig *rig, u32 palette)
{
    s32 i = 0;
    u32 byteMask = 255;
    s32 amplitude = 24;
    for (;;) {
        u32 product = sizeof(struct SceneRigSprite);
        s32 index;
        struct SceneRigSprite *sprite;
        struct SceneRigSprite *output;
        struct ContactPackedVector base;
        union {
            s32 near;
            const s16 *wave;
        } work;
        s32 far;
        s32 lift;
        s32 radius;
        index = i;
        product *= index;
        {
            u32 arrayOffset = (u32) & ((struct ScenePanelRig *)0)->secondary;
            product += arrayOffset;
        }
        sprite = (void *)((u8 *)rig + product);
        output = sprite;
        work.near = index;
        work.near += 2;
        {
            s32 total = 8;
            far = total - work.near;
        }
        {
            s32 difference = rig->position.y;
            difference -= rig->origin.y;
            difference *= far;
            lift = scale5(3 * difference);
        }
        base.x = scale3(rig->position.x * work.near + rig->origin.x * far) + lift;
        base.y = scale3(rig->position.y * work.near + rig->origin.y * far);
        base.z = scale3(rig->position.z * work.near + rig->origin.z * far) + lift;
        {
            register u16 *randomPhase asm("r3");
            u16 *phase;
            work.wave = gUnk_086149C4;
            randomPhase = &sprite->randomPhase;
            {
                register u32 sample asm("r1") = *randomPhase;
                u32 angle = byteMask;
                angle &= sample;
                angle += 64;
                {
                    u32 mask = byteMask;
                    angle &= mask;
                }
                radius = scale12(work.wave[angle] * amplitude);
            }
            {
                s32 horizontal;
                {
                    register u16 *anglePtr asm("r0") = &sprite->phase;
                    s32 product = *anglePtr + 64;
                    {
                        u32 mask = byteMask;
                        product &= mask;
                    }
                    product = work.wave[product] * radius;
                    phase = anglePtr;
                    horizontal = product;
                }
                {
                    u16 x = base.x + scale12(horizontal);
                    struct SceneRigSprite *store = output;
                    store->position.x = x;
                }
            }
            output->position.y = base.y;
            {
                register u32 sample asm("r1") = *phase;
                u32 angle = byteMask;
                s32 offset;
                angle &= sample;
                offset = scale12(work.wave[angle] * radius);
                {
                    u16 z = base.z + offset;
                    struct SceneRigSprite *store = output;
                    store->position.z = z;
                }
            }
            {
                u32 angle = *phase + 3;
                u32 mask;
                mask = byteMask;
                *phase = angle & mask;
            }
            *randomPhase += 5;
        }
        stepSecondarySprite(sprite, byteMask);
        AnimData_SelectPaletteBank(sprite->graphics, palette);
        {
            register s32 next asm("r1") = amplitude;
            next += 4;
            amplitude = next;
            asm("" : : "r"(next));
        }
        {
            register s32 next asm("r2") = i;
            next++;
            i = next;
            if (next > 5)
                break;
        }
    }
}
