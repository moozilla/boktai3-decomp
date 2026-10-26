#include "scene_panel_rig_setup.h"

// MATCHING_CONCESSION: register-pinning
void ScenePanel_InitRig(struct ScenePanel *input)
{
    struct SceneRigSetupParentView *panel = (void *)input;
    struct ScenePanelRig *rig = &panel->rig;
    register u32 next asm("r1");
    u32 zero;
    s32 power;
    rig->origin = panel->center;
    rig->origin.x -= 32;
    rig->origin.z -= 32;
    rig->position = rig->origin;
    rig->position.y += 128;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    Mod(*(u16 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2), 600);
    gRandomIndex = (gRandomIndex + 1) & 1023;
    ScenePanelRig_InitSprites(rig);
    if (Script_SeekToKeyword('L'))
        power = Script_GetValue();
    else
        power = 5;
    ScenePanelRig_InitCollider(rig, power);
    ScenePanelRig_SetState(rig, 0);
    {
        u16 *angle = &rig->angle;
        u32 initialZero = 0;
        zero = initialZero;
        *angle = 160;
    }
    rig->turnStep = zero;
    {
        const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
        u32 *randomIndex;
        u32 mask;
        s32 rate;
        s16 *rateField;
        u32 mask63;
        asm("" : "+r"(table));
        {
            u32 *initialIndex = &gRandomIndex;
            randomIndex = initialIndex;
            next = *initialIndex + 1;
        }
        mask = 1023;
        next &= mask;
        {
            u32 sample = table[next];
            sample &= 15;
            rate = sample;
            rate += 8;
        }
        rateField = &rig->turnRate;
        *rateField = rate;
        next = (next + 1) & mask;
        *randomIndex = next;
        {
            u32 sample = table[next];
            sample &= 1;
            if (sample) {
                s32 negative = -rate;
                *rateField = negative;
            }
        }
        {
            u32 *base = randomIndex;
            next = (*base + 1) & mask;
        }
        {
            u32 sample = table[next];
            mask63 = 63;
            sample &= mask63;
            sample += 192;
            rig->turnDuration = sample;
        }
        rig->motionFlag = 0;
        rig->distance = zero;
        next = (next + 1) & mask;
        {
            u32 sample = *(u8 *)&table[next];
            sample += 192;
            rig->speed = sample;
        }
        next = (next + 1) & mask;
        *randomIndex = next;
        {
            u32 sample = table[next];
            sample &= mask63;
            sample += 64;
            rig->verticalStep = sample;
        }
    }
    {
        u16 *duration = &rig->selectionTimer;
        u32 value;
        value = 600;
        *duration = value;
    }
    ScenePanelRig_InitPayloadVectors(rig);
    rig->parent = input;
}
