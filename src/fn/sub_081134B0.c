#include "global.h"

struct EffPos {
    s16 x, y, z, w;
};

struct ActorA {
    u8 pad0[0x10];
    s32 x, y, z;
    u8 pad1c[0x38 - 0x1c];
    s32 yOff;
};

struct ObjB {
    s32 x, y, z;
};

#define ActorEffect_Spawn    sub_081134B0
#define ObjEffect_SpawnAtPos SpriteEffect_CreateAtPosition

u32 sub_08113398(struct EffPos *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

u32 ActorEffect_Spawn(struct ActorA *a, struct EffPos *off, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6,
                      u32 p7, u32 p8, u32 p9, u32 p10, u32 p11, u32 p12, u32 p13, u32 p14)
{
    struct EffPos posBuf;
    struct EffPos *pos = &posBuf;
    pos->x = a->x >> 12;
    pos->y = ((a->z + (a->y + a->yOff)) >> 12) + 0x60;
    pos->z = a->z >> 12;
    if (off != NULL) {
        pos->x += off->x;
        pos->y += off->y;
        pos->z += off->z;
    }
    return sub_08113398(pos, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
}

u32 ActorParticles_CreateAtOffset(struct ActorA *a, struct EffPos *off, u32 p2, u32 p3, u32 p4,
                                  u32 p5, u32 p6, u32 p7, u32 p8, u32 p9, u32 p10, u32 p11, u32 p12,
                                  u32 p13, u32 p14)
{
    struct EffPos posBuf;
    struct EffPos *pos = &posBuf;
    pos->x = a->x >> 12;
    pos->y = ((a->z + (a->y + a->yOff)) >> 12) + 0x60;
    pos->z = a->z >> 12;
    if (off != NULL) {
        pos->x += off->x;
        pos->y += off->y;
        pos->z += off->z;
    }
    return sub_08113398(pos, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
}

u32 ObjEffect_SpawnAtPos(struct ObjB *a, struct EffPos *off, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6,
                         u32 p7, u32 p8, u32 p9, u32 p10, u32 p11, u32 p12, u32 p13, u32 p14)
{
    struct EffPos posBuf;
    struct EffPos *pos = &posBuf;
    pos->x = a->x >> 12;
    pos->y = ((a->z + a->y) >> 12) + 0x60;
    pos->z = a->z >> 12;
    if (off != NULL) {
        pos->x += off->x;
        pos->y += off->y;
        pos->z += off->z;
    }
    return sub_08113398(pos, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
}
