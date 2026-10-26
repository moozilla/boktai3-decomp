#include "global.h"
#include "engine.h"

// foundry draft (worker best 91.7%); what differs is mostly register allocation and instruction order
#define gEffectRandomIndex gRandomIndex
#define gEffectRandomTable gUnk_0203B400

struct EffectMotion {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
struct EffectSourceObject {
    u8 unk00[0xc];
    struct EffectMotion position;
};
extern const u16 gUnk_0203B400[];
s32 Div(s32, s32);
s32 SpriteEffect_SpawnWithMotion(struct EffectSourceObject *, struct EffectSourceObject *,
                                 struct EffectMotion *, u32, u32, u32, u32);

s32 SpriteEffect_SpawnTowardObject(struct EffectSourceObject *source,
                                   struct EffectSourceObject *target, u32 a, u32 b, u32 c, u32 d)
{
    s32 k2 = 1023;
    struct EffectMotion motion;
    s32 duration;
    const u16 *tbl;

    motion.x = target->position.x - source->position.x;
    motion.z = target->position.z - source->position.z;
    tbl = gUnk_0203B400;
    gRandomIndex = (gRandomIndex + 1) & k2;
    {
        u32 v = tbl[gRandomIndex];
        u32 m = 63;
        duration = (v & m) + 30;
    }
    motion.x = Div(motion.x, duration);
    motion.z = Div(motion.z, duration);
    {
        s32 q = Div(duration, 2);
        u32 v = tbl[gRandomIndex = (gRandomIndex + 1) & 1023];
        u32 m = 7;
        motion.y = q + (v & m);
    }
    if (motion.x > 48) {
        motion.x = 48;
    }
    if (motion.x < (-48)) {
        motion.x = -48;
    }
    if (motion.z > 48) {
        motion.z = 48;
    }
    if (motion.z < (-48)) {
        motion.z = -48;
    }
    return SpriteEffect_SpawnWithMotion(source, target, &motion, a, b, c, d);
}
