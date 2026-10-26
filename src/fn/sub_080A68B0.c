#include "global.h"
#include "scene_actor_reset.h"
/* Tentative: winds up a segment-spread attack while backing away from the target. */
#define TentacleBody_UpdateSpreadPreparation sub_080A68B0
extern const s16 gUnk_086149C4[];
void TentacleBody_UpdateChargeSpread(void);
void sub_080A9DB0(void *, s32, s32);
void ActorTrail_ApplyAngularOffsets(void *, s32, s32, s32);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
// confidence=low: winds up a segment-spread attack while backing away from the target.
static inline void ClearMotion(const u32 *source, void *destination)
{
    CpuSet(source, destination, 0x05000002);
}
void TentacleBody_UpdateSpreadPreparation(struct SceneActorRegistryResetView *actor)
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
    ClearMotion(&zero, dst);
    if ((s16)actor->field2EA > 255) {
        void *target = (void *)TentacleBody_UpdateChargeSpread;
        u32 kind = 0x23;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = target;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        return;
    }
    if ((s32)actor->counter2AC <= 29) {
        speed = 1;
        actor->field2EA += 10;
    } else {
        speed = 2;
        actor->field2EA += 40;
    }
    angle = (actor->direction + 128) & 255;
    actor->motion.x += Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    actor->motion.z += Shift(gUnk_086149C4[angle] * speed, 12);
    sub_080A9DB0(actor, 10, 0x1000);
    ActorTrail_ApplyAngularOffsets(actor, 20, 24, 0);
}
