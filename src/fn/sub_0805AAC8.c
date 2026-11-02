#include "scene_panel_rig.h"
#include "player_cue_sequence.h"
void ScenePanelRig_TickPrimarySprites(struct ScenePanelRig *, u32);
void ScenePanelRig_TickSecondarySprites(struct ScenePanelRig *, u32);
void ScenePanelRig_TickSprites(struct ScenePanelRig *rig, u32 palette)
{
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
            u32 index = (gRandomIndex + 1) & 1023;
            u32 sample;
            gRandomIndex = index;
            sample = *(const u16 *)(index * 2 + (u32)gRandomTable);
            offset = (sample & 15) - 8;
        }
        rig->main.position.x = x + offset;
        rig->main.position.z = z - offset;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&rig->main, &rig->main.sequence);
    AnimData_SelectPaletteBank(rig->main.graphics, palette);
    ScenePanelRig_TickPrimarySprites(rig, palette);
    ScenePanelRig_TickSecondarySprites(rig, palette);
}
