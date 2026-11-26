#include "objects/pile_driver/scene_panel_subject_tail.h"
s32 PileDriverSubject_Destroy(struct ScenePanelSubject *subject)
{
    Sprite_Remove(&subject->drawable);
    Collider_Remove(&subject->collider);
    SceneActorEffects_DestroyGroup(subject->effects);
    return 0;
}
