#include "scene_panel.h"
#define ScenePanel_BeginBackgroundBurst sub_080586C0
void ScenePanel_BeginBackgroundBurst(void)
{
    if (gUnk_020004A4) {
        gUnk_020004A4->mode = 64;
        gUnk_020004A4->shakeOffset = 0;
        gUnk_020004A4->effectPhase = 1;
        Sound_PlaySong(297);
    }
}
