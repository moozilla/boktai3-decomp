#define gNamedObjectSineTable gUnk_086149C4
#include "global.h"
#include "contact_actor.h"
struct NamedObjectMotion {
    u8 unknown00[9];
    u8 direction;
    u8 unknown0A[2];
    struct ContactPosition position;
};
struct NamedObjectTargetPoint { u16 x, z; };
struct NamedObjectTarget {
    u8 unknown00[8];
    struct NamedObjectTargetPoint *point;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
static inline s32 Abs(s32 x) { return x < 0 ? -x : x; }
static inline s32 Scale(s32 x) { if (x >= 0) x >>= 12; else x = -((-x) >> 12); return x; }
static inline s32 ScaleZ(s32 x) { if (x >= 0) return x >> 12; return -((-x) >> 12); }
u32 NamedObject_StepTowardTarget(struct NamedObjectMotion *body, struct NamedObjectTarget *target,
                  struct ContactPosition *step, u32 nearX, u32 nearZ, s32 speed)
{
    s32 dx, dz;
    Memory_Clear(step, 8);
    dx = target->point->x - body->position.x;
    dz = target->point->z - body->position.z;
    if ((u32)Abs(dx) <= nearX && (u32)Abs(dz) <= nearZ) {
        body->position.x += dx;
        body->position.z += dz;
        step->x = dx;
        step->z = dz;
        return 1;
    }
    if (dx || dz) {
        dx = (u8)(((((u8)Math_Atan2(dx, dz) + 0x60) >> 6) << 6) - 0x40);
        body->direction = dx;
        {
        s32 x, z;
        x = Scale(gUnk_086149C4[(dx + 0x40) & 0xFF] * speed);
        z = ScaleZ(gUnk_086149C4[dx] * speed);
        body->position.x += x;
        body->position.z += z;
        step->x = x;
        step->z = z;
        }
    }
    return 0;
}
