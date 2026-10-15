#include "hit_switch.h"
#define HitSwitch_InitCollider sub_080547B0
struct HitSwitchExtentHalf {
    u32 lo : 16;
    u32 hi : 16;
};
struct HitSwitchExtent {
    struct HitSwitchExtentHalf a, b;
};
void sub_0805431C(struct SceneCollider *, void *, struct HitSwitchEntry *);
void HitSwitch_InitCollider(struct HitSwitchEntry *entry)
{
    struct HitSwitchExtent size, offset;
    struct SceneCollider *collider = &entry->collider;
    size.a.lo = 0x80;
    size.a.hi = 0x78;
    size.b.lo = 0x80;
    *(u32 *)&offset.a = 0x7f0000;
    *(u32 *)&offset.b &= 0xffff0000;
    Collider_Init(collider, entry->collisionId, 0x4000, 0, 0x10, &size, &offset);
    Collider_SetCallback(collider, sub_0805431C, entry);
    Collider_SetPosition(collider, &entry->world, 0);
    Collider_Refresh(collider);
}
