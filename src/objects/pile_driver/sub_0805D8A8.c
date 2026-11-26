#include "objects/pile_driver/pile_driver_subject_tail.h"
void PileDriverSubject_InitCollider(struct PileDriverSubject *subject, u32 value)
{
    struct PileDriverInteractionPackedPosition size, offset;
    struct ContactCollider *collider = &subject->collider;
    size.x = 100;
    size.y = 200;
    size.z = 100;
    *(u32 *)&offset = 0xC80000;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x4001, 0, 0x10, &size, &offset);
    Collider_SetBodyProperties(collider, value, 0, 0);
    Collider_SetPosition(collider, &subject->home, 0);
    Collider_SetCallback(collider, PileDriverSubject_ApplyContact, subject);
}
