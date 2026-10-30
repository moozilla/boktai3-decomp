#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"

// confidence=low: follows player displacement while the participant-progress control runs.

void ActorBehavior_TickParticipantProgress(void);
void SceneActorControl_OnIdle(void);
void SceneActorControl_ResetTargetFlags(void *);
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 kind)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = 0;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior,
                            u32 zero)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = zero;
    a->field2B1 = 1;
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
void Lun_FollowParticipantDisplacement(struct SceneActorRegistryResetView *a)
{
    u8 **new_var;
    u8 *ctx = a->context;
    struct ContactPosition delta;
    if (TakeFlag(&a->field2B1)) {
        SetControl(a, ActorBehavior_TickParticipantProgress, 0x23);
        *(struct ContactPosition *)(ctx + 0x414) =
            *(struct ContactPosition *)((*(u8 **)gPlayers) + 0x30);
        SceneActorControl_ResetTargetFlags(a);
    }
    {
        u8 **players = &(*(u8 **)gPlayers);
        Delta(&delta, (void *)(ctx + 0x414), (void *)((*(new_var = &(*(u8 **)gPlayers))) + 0x30));
    }
    a->body.motion.x += delta.x;
    a->body.motion.z += delta.z;
    *(struct ContactPosition *)(ctx + 0x414) =
        *(struct ContactPosition *)((*(u8 **)gPlayers) + 0x30);
    if (a->field2B0) {
        SetState(a, 0, 3, 0);
        SetControl(a, SceneActorControl_OnIdle, 0x10);
    } else {
        a->counter2AC++;
    }
}
