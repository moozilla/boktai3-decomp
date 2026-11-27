#include "monsters/ooze.h"
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Ooze_OnCollision(void);
void Ooze_InitAttackCollider(struct Ooze *p)
{
    struct ContactPackedVector size, offset;
    void *collider = &p->collider;
    if (p->variant == 0) {
        size.x = 128;
        size.y = 128;
        size.z = 128;
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
    } else if (p->facing == 0) {
        size.x = 160;
        size.y = 128;
        size.z = 128;
        offset.x = 128;
        offset.y = 0;
        offset.z = 0;
    } else {
        size.x = 128;
        size.y = 128;
        size.z = 160;
        offset.x = 0;
        offset.y = 0;
        offset.z = 128;
    }
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(collider, Ooze_OnCollision, p);
    Collider_SetAttackProperties(collider, p->extentX, p->extentY, 0, 0, p->extentZ);
}
