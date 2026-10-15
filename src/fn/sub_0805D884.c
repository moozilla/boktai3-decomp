#include "scene_panel_subject_tail.h"
#define ScenePanelSubject_Destroy sub_0805D884
s32 ScenePanelSubject_Destroy(struct ScenePanelSubjectTail *subject)
{
    Sprite_Remove(&subject->drawable);
    Collider_Remove(&subject->collider);
    sub_0823233C(subject->effects);
    return 0;
}
