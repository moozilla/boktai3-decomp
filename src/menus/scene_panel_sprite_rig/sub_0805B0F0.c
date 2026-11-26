#include "engine/shared/monster_particle_random.h"
#include "menus/scene_panel_sprite_rig/scene_panel_rig_setup.h"

void ScenePanel_InitRig(struct ScenePanel *input)
{
    struct ScenePanel *panel = input;
    struct ScenePanelRig *rig = &panel->rig;
    s32 rate;
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
    rig->angle = 160;
    rig->turnStep = 0;
    {
        const u16 *table;
        u32 mask;
        u32 *seed;
        {
            u32 first, second;
            table = gRandomTable;
            seed = &gRandomIndex;
            first = *seed + 1;
            mask = 1023;
            first &= mask;
            {
                u32 sample;
                PARTICLE_RANDOM_READ_SAMPLE(sample, table, first);
                sample &= 15;
                rate = sample + 8;
            }
            rig->turnRate = rate;
            second = (first + 1) & mask;
            *seed = second;
            {
                u32 sample = table[second];
                sample &= 1;
                if (sample)
                    rig->turnRate = -rate;
            }
        }
        {
            u32 first = (*seed + 1) & mask;
            u32 second, third;
            {
                u32 sample = table[first];
                sample &= 63;
                rig->turnDuration = sample + 192;
            }
            rig->motionFlag = 0;
            rig->distance = 0;
            second = (first + 1) & mask;
            rig->speed = *(u8 *)&table[second] + 192;
            third = (second + 1) & mask;
            *seed = third;
            {
                u32 sample = table[third];
                sample &= 63;
                rig->verticalStep = sample + 64;
            }
        }
    }
    rig->selectionTimer = 600;
    ScenePanelRig_InitPayloadVectors(rig);
    rig->parent = input;
}
