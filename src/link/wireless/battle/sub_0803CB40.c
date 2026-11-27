#include "global.h"
#include "link/wireless/wireless_knockback_actor.h"

struct WirelessHitEntry {
    u8 unk0[0x10];
    s16 x;
    u8 unk12[2];
    s16 z;
    u8 unk16[0xA];
    s16 offsetX;
    u8 unk22[2];
    s16 offsetZ;
};
struct Vec4i {
    s32 x, y, z, w;
};
struct Vec3s {
    s16 x, y, z;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);

static inline int WirelessHit_Scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void WirelessHit_Polar(struct Vec3s *v, int angle, int radius)
{
    v->x = WirelessHit_Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = WirelessHit_Scale(gUnk_086149C4[angle & 255] * radius);
}

/* Hit-volume contact callback: pushes the object away from the other
   entry along the (32-step quantized) angle between them. */
void sub_0803CB40(struct WirelessKnockbackActor *pArg, struct WirelessHitEntry *otherArg)
{
    struct WirelessHitEntry *other = otherArg;
    struct WirelessKnockbackActor *p = pArg;
    struct Vec4i them, self, delta;
    struct Vec3s dir;
    int angle;
    int sx, sz, tx, tz, dx, dz;
    self.x = sx = p->posX + p->offX;
    self.z = sz = p->posZ + p->offZ;
    them.x = tx = other->x + other->offsetX;
    them.z = tz = other->z + other->offsetZ;
    delta.x = dx = sx - tx;
    delta.z = dz = sz - tz;
    angle = Math_Atan2(dx, dz);
    angle = (angle >> 5) << 5;
    WirelessHit_Polar(&dir, angle, 8);
    p->velX = dir.x << 10;
    p->velZ = dir.z << 10;
    if ((u32)(angle - 10) > 0xEC) {
        p->f5AC += 0x3000;
        p->unk5EC = 10;
    } else {
        p->f5AC -= 0x2000;
        p->unkC0 = 0;
        p->unk5EC = 15;
    }
}
