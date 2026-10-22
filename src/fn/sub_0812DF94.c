#include "global.h"
#include "scene_actor_placement.h"
int Math_GetVectorLength(int x, int y);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
struct Anchor {
    u8 pad[8];
    u16 *point;
};
struct Base {
    u8 pad0[0xc];
    struct ContactPosition position;
    u8 pad14[0x14];
    struct Anchor *anchor;
};

static inline void ClearFlags16(u16 *f, u32 mask)
{
    *f &= mask;
}
void sub_0812DF94(u8 *p, struct ContactPosition *target)
{
    struct Base *b = (struct Base *)(p + 0x50);
    s32 dx = b->anchor->point[0] - b->position.x;
    s32 dz = b->anchor->point[1] - b->position.z;
    s32 len = (s16)Math_GetVectorLength((s16)dx, (s16)dz);
    void *mesh;
    struct ContactPosition *pos;
    s32 r;
    dx = target->x - b->position.x;
    dz = target->z - b->position.z;
    if (len <= (s16)Math_GetVectorLength((s16)dx, (s16)dz)) {
        target->x = b->anchor->point[0];
        target->y = 0;
        target->z = b->anchor->point[1];
    }
    mesh = *(void **)(p + 0x5d0);
    pos = (struct ContactPosition *)(p + 0x5c);
    if (!Nav_IsPositionInMesh(mesh, pos, target)) {
        ClearFlags16((u16 *)(p + 0x2d4), ~0x200);
    } else {
        r = Terrain_ApplyBlocked(mesh, pos, target);
        if (r != 0) {
            if (r != 2)
                Memory_Clear(p + 0x64, 8);
        } else {
            ClearFlags16((u16 *)(p + 0x2d4), ~0x200);
        }
    }
}
