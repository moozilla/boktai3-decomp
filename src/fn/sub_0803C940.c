#include "global.h"

#define WirelessObj_ApplyKnockback sub_0803C940

/* tentative: applies knockback velocity away from attacker and stun timer */

struct Vec3s {
    s16 x, y, z, w;
};
struct Vec3i {
    s32 x, y, z, w;
};

struct Msg {
    u8 pad0[4];
    s32 type;
    s32 kind;
    u16 value;
    u8 pad0e[2];
    s16 posX, posY, posZ;
    u8 pad16[0x20 - 0x16];
    s16 offX, offY, offZ;
    u8 pad26[0x36 - 0x26];
    u8 speed;
};

struct Obj {
    u8 pad0[0x68];
    s32 value;
    s32 type;
    u8 pad70[0xC0 - 0x70];
    s32 unkC0;
    u8 padC4[0x338 - 0xC4];
    u16 unk338;
    u8 pad33A[0x57A - 0x33A];
    s8 unk57A;
    u8 pad57B;
    s16 posX, posY, posZ;
    u8 pad582[0x58C - 0x582];
    s16 offX, offY, offZ;
    u8 pad592[0x5AC - 0x592];
    s32 unk5AC;
    u8 pad5B0[0x5CC - 0x5B0];
    s32 velX;
    u8 pad5D0[4];
    s32 velZ;
    u8 pad5D8[0x5EC - 0x5D8];
    s8 unk5EC;
    u8 pad5ED;
    u8 flags;
};

extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);

static inline s32 scaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline u8 isBusy(struct Obj *obj)
{
    u32 mask = 1;
    if (obj->flags & mask)
        return TRUE;
    return FALSE;
}

static inline bool32 isKind(s32 kind, s32 want)
{
    if (kind == want)
        return TRUE;
    return FALSE;
}

static inline void polar(struct Vec3s *v, s32 angle, s32 speed)
{
    v->x = scaleMotion(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = scaleMotion(gUnk_086149C4[angle & 255] * speed);
}
void SceneMotion_PlayKindSound(struct Msg *);
void WirelessObj_ApplyKnockback(struct Obj *objArg, struct Msg *msgArg)
{
    struct Msg *msg = msgArg;
    struct Obj *obj = objArg;
    struct {
        struct Vec3i target;
        struct Vec3i self;
        struct Vec3i diff;
    } l;
    struct Vec3s vel;
    s32 angle;
    s32 t;

    if (isBusy(obj))
        goto sound;
    if (msg->type == 7) {
        obj->unk57A = 0;
        goto sound;
    }
    if (isKind(msg->kind, 4) || isKind(msg->kind, 3)) {
        obj->unk57A = 0;
        goto sound;
    }
    obj->value = msg->value;
    obj->type = msg->type;
    {
        u8 one = 1;
        obj->flags |= one;
    }
    l.self.x = obj->posX + obj->offX;
    l.self.y = obj->posY + obj->offY;
    l.self.z = obj->posZ + obj->offZ;
    l.target.x = msg->posX + msg->offX;
    l.target.y = msg->posY + msg->offY;
    l.target.z = msg->posZ + msg->offZ;
    l.diff.x = l.self.x - l.target.x;
    l.diff.y = l.self.y - l.target.y;
    l.diff.z = l.self.z - l.target.z;
    angle = Math_Atan2(l.diff.x, l.diff.z);
    angle = (angle >> 5) << 5;
    polar(&vel, angle, msg->speed);
    t = obj->unk338 >> 1;
    obj->velX = vel.x << 10;
    obj->velZ = vel.z << 10;
    obj->unk5EC = 60 - t;
    if (obj->unk5EC < 10)
        obj->unk5EC = 10;
    if ((u32)(angle - 10) > 0xEC) {
        obj->unk5AC += 0x3000;
    } else {
        obj->unk5AC -= 0x100;
        obj->unkC0 = 0;
    }
sound:
    SceneMotion_PlayKindSound(msg);
}
