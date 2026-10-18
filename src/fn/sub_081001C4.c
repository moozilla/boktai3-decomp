#define WoodenCrate_InitCollider sub_081001C4
#define WoodenCrate_ReactToHit   sub_08100144
#include "global.h"
#include "engine.h"
struct CratePackedVector {
    s32 x : 16, y : 16, z : 16;
};
struct CrateCollider {
    u8 pad0[4];
    u16 id;
    u8 pad6[0x44 - 6];
};
struct WoodenCrate {
    u8 pad0[0x1C];
    u32 position[2];
    u8 pad24[0x60 - 0x24];
    struct CrateCollider collider;
    u8 padA4[0xB4 - 0xA4];
    u32 colliderId;
};
void sub_08100144(void *, void *, struct WoodenCrate *);
void sub_081001C4(struct WoodenCrate *p)
{
    struct CratePackedVector bounds, offset;
    struct CrateCollider *c = &p->collider;
    bounds.x = 0;
    bounds.y = 128;
    bounds.z = 0;
    offset.x = 64;
    offset.y = 128;
    offset.z = 64;
    c->id = p->colliderId;
    Collider_Init(c, 0, 0x4001, 0, 16, &offset, &bounds);
    Collider_SetPosition(c, &p->position, 0);
    Collider_SetCallback(c, sub_08100144, p);
    Collider_Refresh(c);
}
