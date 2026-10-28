// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
#include "engine.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
void Bee_UpdateFacingAnimation(struct SceneActorAnimationView *actor);
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
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
        fp = &actor->fieldE9;
        angle = (r & 0x1F) + 0x71;
        angle = (angle + *fp) & 0xFF;
        {
            struct ContactPosition *m = &actor->secondaryMotion;
            m->x = Shift(gUnk_086149C4[(angle + 0x40) & 0xFF] * 0x20, 12);
            m->y = 0;
            {
                s32 v = gUnk_086149C4[angle];
                s32 k = 0x20;
                asm("" : "+r"(k));
                m->z = Shift(v * k, 12);
            }
        }
    }
    cnt = actor->counter2AC;
    if (cnt > 0x1E) {
        u8 zero8;
        u32 zero;
        asm("" : "+r"(actor));
        actor->flags2D4 = Mask16(&actor->flags2D4, ~0x100);
        zero8 = 0;
        zero = 0;
        {
            u32 two = 2;
            actor->state2A8 = zero8;
            actor->phase2A9 = two;
            actor->behavior2AA = zero8;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
        }
    } else {
        actor->counter2AC = cnt + 1;
    }
}
