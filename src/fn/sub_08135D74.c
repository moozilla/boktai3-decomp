#include "global.h"
/* SceneActorReaction_TryStartControl1D: when flag 0x08000000 (+0x2D0) and 0x2000 (+0x2D4) are set, not blocked by 0x8006 and the +0x2B8 timer runs, switches to control sub_08081C98 (kind 0x1D) and clears collision state; returns 1 if in that control */
#define SceneActorReaction_TryStartControl1D sub_08135D74
#include "scene_actor_reset.h"
void sub_08081C98(void);
void SceneActorTerrain_ClearCollisionTimers(void *);
void sub_0807C72C(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);

static inline void SetControl(struct SceneActorRegistryResetView *actor)
{
    void *callback = sub_08081C98;
    u8 kind = 0x1D;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u32 Masked(u16 *p, u32 mask)
{
    return mask & *p;
}

static inline u8 TestFlags(u32 mask, u16 *p)
{
    if (mask & *p)
        return 1;
    return 0;
}

u8 SceneActorReaction_TryStartControl1D(struct SceneActorRegistryResetView *actor)
{
    u32 *flags2D0;
    u16 *flags2D4;
    u32 blocked;
    struct ContactCollider *collider;

    u32 bit = 0x08000000;
    flags2D0 = &actor->flags2D0;
    if (*flags2D0 & bit) {
        u32 value;
        blocked = 0x8006;
        {
            u16 *p = &actor->flags2D4;
            value = *p;
            blocked &= value;
            flags2D4 = p;
        }
        if (!blocked && (value & 0x2000) && actor->field2B8) {
            if (actor->field2B5 == 0x1D)
                return 1;
            SetControl(actor);
            SetAnimation(actor, 3, 0, 0);
            SceneActorTerrain_ClearCollisionTimers(actor);
            {
                s32 mask = ~0x13;
                *flags2D4 &= mask;
            }
            {
                u32 mask = ~2;
                *flags2D0 &= mask;
            }
            {
                u16 *flags = &actor->flags2D6;
                s32 mask = ~0x9CB;
                *flags &= mask;
            }
            sub_0807C72C(actor, 0, 0x7F, 0);
            sub_0807C72C(actor, 1, 0x7F, 0);
            sub_0807C72C(actor, 3, 0x7F, 0);
            SceneActorEffects_AdvanceSteps(actor, 4);
            actor->field5C4 = 0;
            actor->field5C0 = 0;
            actor->field5C2 = 0;
            collider = &actor->colliderA;
            if (TestFlags(0x40, &actor->flags2C0))
                collider->flags &= 0xFEFF;
            else if ((actor->field2DC & 0x84000) || Masked(flags2D4, 0x2016))
                collider->flags &= 0xFEFF;
            else
                collider->flags |= 0x100;
            return 1;
        }
    }
    return 0;
}
