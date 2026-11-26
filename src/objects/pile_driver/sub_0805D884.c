#include "objects/pile_driver/pile_driver_subject_tail.h"
s32 PileDriverSubject_Destroy(struct PileDriverSubject *subject)
{
    Sprite_Remove(&subject->drawable);
    Collider_Remove(&subject->collider);
    SceneActorEffects_DestroyGroup(subject->effects);
    return 0;
}
