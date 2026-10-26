// MATCHING_CONCESSION: register-pinning
#include "scene_panel_rig.h"
void ScenePanelRig_TickPrimarySprites(struct ScenePanelRig *, u32);
void ScenePanelRig_TickSecondarySprites(struct ScenePanelRig *, u32);
void ScenePanelRig_TickSprites(struct ScenePanelRig *rig, u32 palette)
{
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    u32 x, z;
    rig->main.position = rig->position;
    x = (u16)rig->main.position.x + 44;
    rig->main.position.x = x;
    rig->main.position.y += 64;
    z = (u16)rig->main.position.z + 44;
    rig->main.position.z = z;
    if (rig->state == 8) {
        s32 offset;
        {
            register s32 sample asm("r0");
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            sample = index << 1;
            sample = *(const u16 *)((u32)gUnk_0203B400 + sample);
            sample &= 15;
            sample -= 8;
            offset = sample;
        }
        rig->main.position.x = x + offset;
        rig->main.position.z = z - offset;
    }
    state = &rig->main.sequence;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = rig->main.sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    rig->main.frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            rig->main.flags |= 4;
        else
            rig->main.flags &= ~4;
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
            rig->main.flags |= 8;
        else
            rig->main.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r6");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
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
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
    AnimData_SelectPaletteBank(rig->main.graphics, palette);
    ScenePanelRig_TickPrimarySprites(rig, palette);
    ScenePanelRig_TickSecondarySprites(rig, palette);
}
