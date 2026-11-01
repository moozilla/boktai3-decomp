#include "scene_actor_control_phase.h"
#include "engine.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
void Bee_UpdateFacingAnimation(struct SceneActorAnimationView *actor);
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR);
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u16 Mask16(u16 *p, u32 m)
{
    return *p & m;
}
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Polar(struct ContactPosition *m, u32 angle, s32 radius)
{
    m->x = Shift(gUnk_086149C4[(angle + 0x40) & 0xFF] * radius, 12);
    m->y = 0;
    m->z = Shift(gUnk_086149C4[angle] * radius, 12);
}
void Bee_UpdateWanderPause(struct SceneActorRegistryResetView *actor)
{
    s32 cnt;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = (void *)Bee_UpdateFacingAnimation;
        u32 kind = 0x27;
        u32 zero;
        u32 angle;
        u32 r;
        u8 *fp;
        u8 *taken = &actor->field2B2;
        zero = 0;
        *taken = 1;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
        r = Rand();
        fp = &actor->tracking.heading;
        angle = (r & 0x1F) + 0x71;
        angle = (angle + *fp) & 0xFF;
        Polar(&actor->secondaryMotion, angle, 0x20);
    }
    cnt = actor->counter2AC;
    if (cnt > 0x1E) {
        actor->flags2D4 = Mask16(&actor->flags2D4, ~0x100);
        SetPhase(actor, 0, 2, 0);
    } else {
        actor->counter2AC = cnt + 1;
    }
}
