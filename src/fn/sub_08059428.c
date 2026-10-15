#include "scene_panel_rig.h"
void ScenePanelRig_ApplyContact(struct SceneRigContact *source, struct SceneRigContact *target,
                                struct ScenePanelRig *input)
{
    u32 reactionMask = 0x7F;
    if (source->reactionFlags & reactionMask) {
        struct ScenePanelRig *rig = input;
        s32 force;
        u32 flags, mask;
        rig->contactDirection = source->direction;
        Sound_PlaySong(243);
        force = source->power - target->power;
        if (force < 0)
            force = 0;
        mask = 1;
        flags = source->typeFlags;
        if (flags & mask) {
            rig->contactForce = force + 20;
            rig->contactEffect = 32;
        } else if (flags & 2) {
            rig->contactForce = force + 100;
            rig->contactEffect = 24;
        } else if (flags & 4) {
            rig->contactForce = force + 50;
            rig->contactEffect = 128;
        } else if (flags & 16) {
            rig->contactForce = force + 100;
            rig->contactEffect = 128;
        } else if (flags & 512) {
            rig->contactForce = 0;
            rig->contactEffect = 128;
        }
        if (rig->state == 5)
            rig->contactEffect = 0;
        if (rig->contactForce > 200)
            rig->contactForce = 200;
    }
}
