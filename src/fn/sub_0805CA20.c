#include "scene_panel_lifecycle.h"
s32 ScenePanel_DestroyLifecycle(struct ScenePanelLifecycleView *parent)
{
    if (parent->initialMode == 0) {
        if (parent->paletteSprite.active)
            Sprite_Remove(&parent->paletteSprite);
    } else {
        ScenePanelRig_RemoveSprites(&parent->rig);
        Sprite_Remove(&parent->auxiliary);
        sub_08215A74(1, 1);
    }
    ScenePanel_DestroyParticles((void *)parent);
    ScenePanel_UnregisterReceiver((void *)parent);
    gUnk_020004A4 = 0;
    return 0;
}
