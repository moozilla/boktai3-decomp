#include "global.h"

// confidence=low: sub_0803CB40's push-away for an actor body: knockback of radius 10 along the
// angle from the other body, held for 30 frames. All three delta components use the x
// difference, as in the original.
struct BodyVolume {
    u8 unk0[0x10];
    s16 x, y, z;
    u8 unk16[0xA];
    s16 offsetX, offsetY, offsetZ;
};
struct KnockbackActor {
    u8 unk0[0x56C];
    struct BodyVolume body;
    u8 unk594[0x5CC - 0x594];
    s32 knockX;
    u8 unk5D0[4];
    s32 knockZ;
    u8 unk5D8[0x14];
    u8 knockTimer;
};
struct Vec4i {
    s32 x, y, z, w;
};
struct Vec3s {
    s16 x, y, z;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);

static inline int Knockback_Scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void Knockback_Polar(struct Vec3s *v, int angle, int radius)
{
    v->x = Knockback_Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Knockback_Scale(gUnk_086149C4[angle & 255] * radius);
}

void ActorMotion_SetKnockback(struct KnockbackActor *p, struct BodyVolume *other)
{
    struct Vec4i them, self, delta;
    struct Vec3s dir;
    struct BodyVolume *body = &p->body;
    int angle;
    self.x = body->x + body->offsetX;
    self.y = body->y + body->offsetY;
    self.z = body->z + body->offsetZ;
    them.x = other->x + other->offsetX;
    them.y = other->y + other->offsetY;
    them.z = other->z + other->offsetZ;
    delta.x = self.x - them.x;
    delta.y = self.x - them.x;
    delta.z = self.x - them.x;
    angle = Math_Atan2(delta.x, delta.z);
    Knockback_Polar(&dir, angle, 10);
    p->knockX = dir.x << 12;
    p->knockZ = dir.z << 12;
    p->knockTimer = 30;
}
