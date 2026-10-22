#include "global.h"

#ifdef NONMATCHING
// Remaining: Only the mask and flag-load registers differ in the pursuit check.
// fuzzy: 97.7%
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"

s32 Mod(s32, s32);
void SceneActorControl_ResetCollisionFlags(struct SceneActorRegistryResetView *);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u16 Mask(struct SceneActorRegistryResetView *p, u32 mask) { return p->flags2D4 & mask; }
// confidence=low: applies stored route drift and halves it every four ticks before choosing pursuit or rest.
#define Lun_DecayRouteMotion sub_0809C174
void Lun_DecayRouteMotion(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    s16 *x, *z;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        SetControl(actor, *(void **)((u8 *)actor + 0x224), 2);
    {
        struct ContactPosition *motion = (void *)((u8 *)actor + 0x64);
        motion->x += *(x = (s16 *)(body + 0x424));
        { s16 *v = &motion->z; *v += *(z = (s16 *)(body + 0x428)); }
    }
    if (Mod(actor->counter2AC, 4) == 0) {
        *x >>= 1;
        *z >>= 1;
    }
    if (actor->field2B0) {
        if (Mask(actor, 0x100))
            SetState(actor, 0, 2, 1);
        else
            SetState(actor, 0, 0, 2);
    } else
        actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0809C174);
#endif
