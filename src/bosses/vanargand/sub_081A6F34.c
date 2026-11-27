#include "bosses/vanargand/vanargand_damage_part.h"
struct ColliderExtentPair {
    u32 lo : 16, hi : 16;
};
struct ColliderExtentTail {
    u32 lo : 16;
    u32 : 16;
};
struct ColliderExtents {
    struct ColliderExtentPair a;
    struct ColliderExtentTail b;
};
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
s32 Collider_Refresh(void *);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Vanargand_OnPartContact(void);
void Vanargand_InitPartCollider(struct VanargandDamagePart *part)
{
    struct ColliderExtents size, offset;
    struct ContactCollider *collider = &part->collider;
    size.a.lo = 128;
    size.a.hi = 256;
    size.b.lo = 128;
    offset.a.lo = 0;
    offset.a.hi = 128;
    offset.b.lo = 0;
    Collider_Init(collider, 0, 0x5001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &part->sprite.position, 0);
    Collider_SetCallback(collider, Vanargand_OnPartContact, part);
    Collider_SetBodyProperties(collider, 100, 2, 1);
    {
        u32 flags = 32;
        part->collider.direction = flags;
    }
    Collider_Refresh(collider);
}
