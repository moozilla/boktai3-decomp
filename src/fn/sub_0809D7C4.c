#include "global.h"

#ifdef NONMATCHING
// Remaining: Entry control-store allocation and player/saved-position address order differ.
// fuzzy: 86.5%
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

void ActorBehavior_TickParticipantProgress(void);
void SceneActorControl_ResetTargetFlags(void *);
extern u8 *gUnk_02000580;
struct Pair { u32 a, b; };
static inline void Delta(struct ContactPosition *d, struct ContactPosition *a, struct ContactPosition *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
// confidence=low: carries Lun by the player's position delta during a participant action.
#define Lun_FollowPlayerDisplacement sub_0809D7C4
void Lun_FollowPlayerDisplacement(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    struct ContactPosition delta;
    struct ContactPosition *player;
    struct ContactPosition *saved;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SetControl(actor, ActorBehavior_TickParticipantProgress, 0x23);
        *(struct Pair *)(body + 0x414) = *(struct Pair *)(gUnk_02000580 + 0x30);
        SceneActorControl_ResetTargetFlags(actor);
    }
    saved = (void *)(body + 0x414);
    player = (void *)(gUnk_02000580 + 0x30);
    Delta(&delta, player, saved);
    (*(struct ContactPosition *)((u8 *)actor + 0x64)).x += delta.x;
    (*(struct ContactPosition *)((u8 *)actor + 0x64)).z += delta.z;
    *(struct Pair *)saved = *(struct Pair *)player;
    if (actor->field2B0) {
        SetState(actor, 0, 0, 3);
        SetControl(actor, SceneActorControl_OnIdle, 16);
    } else
        actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0809D7C4);
#endif
