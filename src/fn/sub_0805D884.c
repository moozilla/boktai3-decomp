#include "scene_panel_subject_tail.h"
s32 ScenePanelSubject_Destroy(struct ScenePanelSubject *subject)
{
    Sprite_Remove(&subject->drawable);
    Collider_Remove(&subject->collider);
    SceneActorEffects_DestroyGroup(subject->effects);
    return 0;
}
