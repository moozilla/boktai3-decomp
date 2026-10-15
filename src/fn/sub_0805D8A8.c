#include "scene_panel_subject_tail.h"
#define ScenePanelSubject_InitCollider sub_0805D8A8
void ScenePanelSubject_InitCollider(struct ScenePanelSubjectView *subject, u32 value)
{
    struct ScenePanelInteractionPackedPosition size, offset;
    struct ContactCollider *collider = &subject->collider;
    size.x = 100;
    size.y = 200;
    size.z = 100;
    *(u32 *)&offset = 0xC80000;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x4001, 0, 0x10, &size, &offset);
    sub_0821FF7C(collider, value, 0, 0);
    Collider_SetPosition(collider, &subject->home, 0);
    Collider_SetCallback(collider, sub_0805CCF0, subject);
}
