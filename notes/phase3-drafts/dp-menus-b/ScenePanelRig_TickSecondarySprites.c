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
        if (first != (((*command & 0x30) >> 4) & 1))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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
        s32 near;
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
        near = index;
        near += 2;
        {
            s32 total = 8;
            far = total - near;
        }
        {
            s32 difference = rig->position.y;
            difference -= rig->origin.y;
            difference *= far;
            lift = scale5(3 * difference);
        }
        base.x = scale3(rig->position.x * near + rig->origin.x * far) + lift;
        base.y = scale3(rig->position.y * near + rig->origin.y * far);
        base.z = scale3(rig->position.z * near + rig->origin.z * far) + lift;
        {
            const s16 *wave = gUnk_086149C4;
            u16 *randomPhase = &sprite->randomPhase;
            u16 *phase;
            {
                u32 sample = *randomPhase;
                u32 angle = byteMask;
                angle &= sample;
                angle += 64;
                {
                    u32 mask = byteMask;
                    angle &= mask;
                }
                radius = scale12(wave[angle] * amplitude);
            }
            {
                s32 horizontal;
                {
                    u16 *anglePtr = &sprite->phase;
                    s32 product = *anglePtr + 64;
                    {
                        u32 mask = byteMask;
                        product &= mask;
                    }
                    product = wave[product] * radius;
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
                u32 sample = *phase;
                u32 angle = byteMask;
                s32 offset;
                angle &= sample;
                offset = scale12(wave[angle] * radius);
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
            s32 next = amplitude;
            next += 4;
            amplitude = next;

        }
        {
            s32 next = i;
            next++;
            i = next;
            if (next > 5)
                break;
        }
    }
}
