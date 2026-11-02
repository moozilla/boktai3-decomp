#ifndef GUARD_SUN_ORB_EFFECT_H
#define GUARD_SUN_ORB_EFFECT_H

#include "contact_collider.h"

struct OrbitEffect;
typedef s32 (*OrbitEffectCallback)(struct OrbitEffect *, s32);
struct OrbitEffectPosition {
    s16 x, y, z, pad;
};
struct OrbitEffectParams {
    u16 unk_00, lifetime;
    s16 kind;
    s8 angle;
    u8 unk_07;
    u32 movementEnabled;
    u8 unk_0C, unk_0D;
    s16 collisionX, collisionY, collisionZ;
    s16 homing;
    u16 unk_16;
    s32 speed;
    OrbitEffectCallback callback;
};
struct OrbitEffectSprite {
    u32 flags;
    u8 unk_04[4];
    u8 scaleX, scaleY;
    u8 unk_0A[0x1C - 0x0A];
    struct OrbitEffectPosition position;
    u8 unk_24[8];
};
struct OrbitTrailSprite {
    u32 flags;
    u8 unk_04[0x18 - 4];
    struct OrbitEffectPosition position;
    u8 unk_20[8];
    struct OrbitEffectPosition *target;
    u16 active, age;
    s16 angle, radius, velocityX, velocityZ;
};
struct OrbitVolleyState {
    s16 lowered, count, phase, unk_06;
};
struct OrbitEffect {
    u32 frame;
    u8 unk_04[8];
    struct OrbitEffectPosition velocity;
    struct OrbitEffectParams params;
    struct OrbitEffectSprite sprite, secondarySprite;
    u32 flags;
    u8 unk_90[0xA0 - 0x90];
    struct ContactCollider collider;
    struct OrbitVolleyState volley;
    void *resource;
    struct OrbitTrailSprite trails[12];
    u16 nextTrail, trailTimer;
    s32 scale;
    u32 unk_3A8;
};
struct OrbitEffectTarget {
    u8 unk_00[0x260];
    struct OrbitEffectPosition position;
    u8 unk_268[0x5C2 - 0x268];
    s16 active;
};
struct OrbitEffectManager {
    u8 unk_00[0x18];
    u32 activeSlots;
    u8 unk_1C[0x38 - 0x1C];
    struct OrbitEffect slots[4];
    u32 unk_EE8, unk_EEC;
    s32 count;
    struct OrbitEffectTarget *target;
};

typedef char OrbitEffectSizeCheck[sizeof(struct OrbitEffect) == 0x3AC ? 1 : -1];
typedef char OrbitTrailSpriteSizeCheck[sizeof(struct OrbitTrailSprite) == 0x38 ? 1 : -1];

void SunOrb_SetUpdateCallback(struct OrbitEffectParams *, OrbitEffectCallback);
s32 RisingActor_AdvanceHeight(struct OrbitEffect *, s32);
s32 RisingActor_LowerHeight(struct OrbitEffect *, s32);
s32 OrbitEffect_FireVolley(struct OrbitEffect *, s32);
s32 OrbitEffect_UpdateExpansion(struct OrbitEffect *, s32);
s32 OrbitEffect_UpdateContraction(struct OrbitEffect *, s32);

void sub_081FDCB0(struct OrbitTrailSprite *, void *resource);

#endif
