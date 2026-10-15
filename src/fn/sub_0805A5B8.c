// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
static inline s32 scale12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline s32 absolute(s32 v)
{
    return v < 0 ? -v : v;
}
void ScenePanelRig_TickPrimarySprites(struct ScenePanelRig *rig, u32 palette)
{
    s32 i;
    for (i = 0;;) {
        struct SceneRigSprite *sprite = &rig->primary[i];
        register struct SceneRigSprite *output asm("r4") = sprite;
        s32 radius;
        radius =
            scale12(absolute(gUnk_086149C4[((sprite->randomPhase & 255) + 64) & 255]) * 40) + 60;
        output->position.x =
            rig->position.x + scale12(gUnk_086149C4[(sprite->phase + 64) & 255] * radius);
        output->position.y = rig->position.y;
        output->position.z = rig->position.z + scale12(gUnk_086149C4[sprite->phase & 255] * radius);
        if (rig->state == 2)
            sprite->randomPhase += 4;
        else
            sprite->randomPhase += 2;
        if (i & 1)
            sprite->phase = (sprite->phase + 1) & 255;
        else
            sprite->phase = (sprite->phase + 255) & 255;
        {
            struct DisplaySequenceState *state = &sprite->sequence;
            const volatile u16 *command;
            u32 duration, flags, bit;
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = sprite->sequence.commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            sprite->frame = *command >> 6;
            {
                register u32 input asm("r0") = state->flags;
                u32 first;
                u32 maskBit = 1;
                bit = maskBit;
                first = bit;
                first &= input;
                if (first != (((*command & 0x30) >> 4) & bit))
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
                if (first != (((*command & 0x30) >> 4) & bit))
                    sprite->flags |= 8;
                else
                    sprite->flags &= ~8;
            }
            {
                u32 elapsed = state->elapsed + 1;
                register u32 mask asm("r5");
                register u32 temp asm("r3");
                register u32 savedDuration asm("r2");
                state->elapsed = elapsed;
                temp = 0xFFFF;
                asm("" : "+r"(temp));
                mask = temp;
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
                        speed &= 255;
                        if (!speed) {
                            state->duration = 1;
                            asm("" : : "r"(duration));
                        }
                    }
                }
            }
        }
        AnimData_SelectPaletteBank(sprite->graphics, palette);
        {
            u32 step = 1;
            i += step;
        }
        {
            register s32 compare asm("r1") = i;
            if (compare > 7)
                break;
        }
    }
}
