#include "global.h"
#include "ram.h"
#include "scene_pool.h"


/* ActorRecoil_ReturnZero: Clears the singleton sprite drop manager pointer */


void ActorRecoil_ReturnZero(void)
{
    gCompactParticlePool = 0;
}

static inline u8 ScenePool_HasControlRestriction(struct ScenePoolActor *actor, u32 mask)
{
    if (actor->flags5ee & mask)
        return 1;
    return 0;
}

static inline u32 ScenePool_TestPlayerFlags(u32 flags, u32 mask)
{
    return flags & mask;
}

u8 ScenePool_CanAcceptControl(struct ScenePoolActor *actor)
{
    if ((actor->linkedPlayer && ScenePool_TestPlayerFlags(actor->linkedPlayer->flags, 4)) ||
        ScenePool_HasControlRestriction(actor, 4))
        return 0;
    return 1;
}

// confidence=low: body-volume callback applies knockback and records special contacts.

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
    u8 pad33A[0x528 - 0x33A];
    u16 contactValue;
    u8 contactKind, contactPending;
    u8 pad52C[0x57A - 0x52C];
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

static inline u8 isBusy(struct Obj *obj, u8 **out)
{
    u32 mask = 1;
    u8 *flags = &obj->flags;
    u32 test = *flags & mask;
    *out = flags;
    if (test)
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

u8 ScenePool_CanAcceptControl();

void SceneMotion_PlayKindSound(struct Msg *);

u16 ModifierStats_ApplyKind15(struct Obj *, u16);

void ActorRecoil_ReceiveBodyContact(struct Obj *objArg, struct Msg *msgArg)
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
    u8 *flags;

    if (!ScenePool_CanAcceptControl(obj) || msg->type == 7) {
        obj->unk57A = 0;
        return;
    }
    if (isBusy(obj, &flags))
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
        *flags |= one;
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
    if (isKind(msg->kind, 4) || isKind(msg->kind, 3)) {
        obj->contactValue = ModifierStats_ApplyKind15(obj, msg->value);
        obj->contactKind = msg->kind;
        obj->contactPending = 1;
    }
}

static inline s32 Scale12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

struct RecoilVec {
    s32 x, y, z, w;
};

struct RecoilVel {
    s16 x, y, z;
};

struct RecoilMsg {
    u8 pad0[8];
    s32 type;
    u8 padc[4];
    s16 x;
    u8 pad12[2];
    s16 z;
    u8 pad16[0xa];
    s16 dx;
    u8 pad22[2];
    s16 dz;
};

extern const s16 gUnk_086149C4[];

u8 ScenePool_CanAcceptControl();

static inline u8 Recoil_IsIgnored(s32 type)
{
    u8 r = 0;
    if (type == 2)
        r = 1;
    return r;
}

static inline void Recoil_SetVelocity(struct RecoilVel *out, s32 angle, s32 k)
{
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xff] * k);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 0xff] * k);
}

void ActorRecoil_ApplyDirectionalImpulse(u8 *actor, struct RecoilMsg *msg)
{
    struct RecoilVec src, dst, diff;
    struct RecoilVel vel;
    s32 angle;
    s32 a, b, c, d;
    if (!ScenePool_CanAcceptControl(actor))
        return;
    if (Recoil_IsIgnored(msg->type))
        return;
    a = *(s16 *)(actor + 0x57C) + *(s16 *)(actor + 0x58C);
    dst.x = a;
    b = *(s16 *)(actor + 0x580) + *(s16 *)(actor + 0x590);
    dst.z = b;
    c = msg->x + msg->dx;
    src.x = c;
    d = msg->z + msg->dz;
    src.z = d;
    diff.x = a - c;
    diff.z = b - d;
    angle = Math_Atan2(a - c, b - d);
    angle = (angle >> 5) << 5;
    Recoil_SetVelocity(&vel, angle, 8);
    *(s32 *)(actor + 0x5CC) = vel.x << 10;
    *(s32 *)(actor + 0x5D4) = vel.z << 10;
    if ((u32)(angle - 10) > 0xEC) {
        *(s32 *)(actor + 0x5AC) += 0x3000;
        actor[0x5EC] = 10;
    } else {
        *(s32 *)(actor + 0x5AC) -= 0x2000;
        *(s32 *)(actor + 0xC0) = 0;
        actor[0x5EC] = 15;
    }
}

// Tentative family and semantic names; layouts follow observed field offsets.

struct CollisionPoint {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};

struct RecoilCollisionActor {
    u8 unknown0[0x18];
    u8 owner[0x544 - 0x18];
    u8 hitVolume[0x40];
    u8 bodyVolume[0xac];
    s16 width, height;
    u8 unknown634[0x8f8 - 0x634];
    s8 group;
};

void ActorRecoil_ReceiveBodyContact();

void ActorRecoil_ApplyDirectionalImpulse();

void SpatialIndex_InitEntry(void *, void *, void *, void *, s32, s32, s32, s32, s32, void (*)(),
                            void *);

void SpatialIndex_InsertEntry(void *);

void ActorRecoil_InitCollisionVolumes(struct RecoilCollisionActor *p)
{
    struct CollisionPoint pos, size, offset;
    void *owner = p->owner;
    s32 group = p->group;
    {
        s32 one = 1;
        group += one;
    }
    pos.x = p->width;
    pos.y = p->height - 96;
    pos.z = p->height - 96;
    size.x = 22;
    size.y = 10;
    size.z = 4;
    offset.x = 0;
    offset.y = -size.y;
    offset.z = 2;
    SpatialIndex_InitEntry(p->bodyVolume, &pos, &size, &offset, 20, group, 0, 0, 12,
                           ActorRecoil_ReceiveBodyContact, owner);
    ((s16 *)&pos)[0] = p->width;
    ((s16 *)&pos)[1] = p->height - 96;
    ((s16 *)&pos)[2] = p->height - 96;
    ((s16 *)&size)[0] = 16;
    ((s16 *)&size)[1] = 8;
    ((s16 *)&size)[2] = 6;
    ((s16 *)&offset)[0] = 0;
    ((s16 *)&offset)[1] = -((s16 *)&size)[1];
    ((s16 *)&offset)[2] = 4;
    SpatialIndex_InitEntry(p->hitVolume, &pos, &size, &offset, 44, group, 0, 15, 12,
                           ActorRecoil_ApplyDirectionalImpulse, owner);
    SpatialIndex_InsertEntry(p->bodyVolume);
}

#define ActorRecoil_UpdateStateSound Character_UpdateLoopingSound

// Tentative family and semantic names; layouts follow observed field offsets.

struct RecoilBody {
    u8 unknown0[0xac];
    u8 flagAC;
    u8 unknownAD[3];
    void *callback;
    u8 unknownB4[0x524 - 0xb4];
    u16 stateSound;
    u16 previousSound;
    u8 unknown528[0x84];
    s32 impulse;
    s32 unknown5B0;
    s32 velocity;
};

struct RecoilMotionActor {
    u8 unknown[0x18];
    struct RecoilBody body;
    u8 unknown5D0[0x8c8 - 0x5d0];
    u8 collisionInfo[1];
};

s32 MotionTween_Advance(struct RecoilBody *, void *);

s32 Callback_InvokeActor(void *, void *);

void ScenePool_ClampForwardBias(struct RecoilBody *);

s32 Div(s32, s32);

u8 LinkParticipant_IsLocallySelected(struct RecoilMotionActor *);

void Character_UpdateLoopingSound(struct RecoilBody *);

s32 ActorRecoil_UpdateMotion(struct RecoilMotionActor *p)
{
    struct RecoilBody *body = &p->body;
    s32 result = MotionTween_Advance(body, p->collisionInfo);
    s32 changed = 0;
    if (Callback_InvokeActor(body, body->callback)) {
        if (body->stateSound == 0x89)
            body->stateSound = 0x8b;
        else
            body->stateSound = 0x8a;
        changed = 1;
    }
    ScenePool_ClampForwardBias(body);
    if (!body->flagAC && !result) {
        body->impulse -= 0x20;
        if (!changed)
            body->stateSound = 0x87;
    }
    body->velocity = Div(body->velocity * 9, 10);
    if (LinkParticipant_IsLocallySelected(p))
        Character_UpdateLoopingSound(body);
    return result;
}
