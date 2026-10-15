#include "scene_panel_subject.h"
void ScenePanelSubject_KeepPlayerOutside(struct ScenePanelSubjectView *subject)
{
    s32 x = SCENE_PANEL_PLAYER->position.x - subject->home.x;
    s32 z = SCENE_PANEL_PLAYER->position.z - subject->home.z;
    if ((x >= 0 ? x : -x) <= 127 && (z >= 0 ? z : -z) <= 127 && x * x + z * z <= 0x3FFF) {
        s32 angle = Math_Atan2(x, z);
        SCENE_PANEL_PLAYER->position.x =
            ((gUnk_086149C4[(angle + 64) & 255] * 65) >> 11) + subject->home.x;
        SCENE_PANEL_PLAYER->position.z =
            ((gUnk_086149C4[angle & 255] * 65) >> 11) + subject->home.z;
    }
}
