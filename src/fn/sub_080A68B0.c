// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_reset.h"
#define SPIN(b) (*((u8 *)(b) + 0x875))
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void sub_080A9DB0(void *, s32, s32);
struct TentacleSegment {
    u8 unknown0[0x1C];
    u16 posX;
    u8 pad2[2];
    u16 posZ;
    u8 pad6[0xE9 - 6];
    u8 flags;
    u8 padEA[0x100 - 0xEA];
    u16 offsetX, offsetY, offsetZ;
    u8 pad106[6];
};
struct TentacleBodyView {
    u8 pad0[0x3D4];
    struct TentacleSegment segments[3];
    u8 pad768[0x8DA - 0x74C];
    s16 segmentCount;
};
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(struct TentacleSegment *s, u32 m)
{
    if (s->flags & m)
        return 1;
    return 0;
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
void TentacleBody_GetDirectionFrame(u16, u32 *, u32 *);
void TentacleBody_RestartSegmentAnimations(void *, u32, u32, u32);
void TentacleBody_UpdateSpreadRelease(void);
static inline void SetBodyFlag(struct TentacleBodyView *body, u32 input)
{
    u32 mask = input;
    u16 *f;
    asm("" : "+r"(mask));
    f = (void *)((u8 *)body + 0x8D0);
    *f = mask | *f;
}
// confidence=low: winds up a segment-spread attack while backing away from the target.
// confidence=low: winds up a segment-spread attack while backing away from the target.
// confidence=low: winds up a segment-spread attack while backing away from the target.
// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.

// confidence=low: winds up a segment-spread attack while backing away from the target.
void TentacleBody_UpdateChargeSpread(void);
void ActorTrail_ApplyAngularOffsets(void *, s32, s32, s32);

#ifdef NONMATCHING
// fuzzy: 80.0%
// scope shape 30 (differences once low registers are set aside)
// confidence=low: winds up a segment-spread attack while backing away from the target.
void sub_080A68B0(struct SceneActorRegistryResetView *actor)
{
    u32 zero;
    struct ContactPosition *dst;
    s32 speed;
    s32 angle;

    if (Take(&actor->field2B2)) {
        u32 mask = 0x100000;
        actor->flags2D0 |= mask;
    }
    dst = &actor->secondaryMotion;
    zero = 0;
    CpuSet(&zero, dst, 0x05000002);
    if ((s16)actor->field2EA > 255) {
        void *target = (void *)TentacleBody_UpdateChargeSpread;
        u32 kind = 0x23;
        actor->field2B2 = 1;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = target;
        actor->field2C8 = 0;
        actor->flags114 &= -2;
        actor->word104 = 0;
        return;
    }
    if ((s32)actor->counter2AC > 29) {
        speed = 2;
        actor->field2EA += 40;
    } else {
        speed = 1;
        actor->field2EA += 10;
    }
    angle = (actor->direction + 128) & 255;
    actor->motion.x += Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    actor->motion.z += Shift(gUnk_086149C4[angle] * speed, 12);
    sub_080A9DB0(actor, 10, 0x1000);
    ActorTrail_ApplyAngularOffsets(actor, 20, 24, 0);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A68B0);
#endif
