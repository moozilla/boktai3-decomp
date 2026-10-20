#include "scene_actor_control_phase.h"
// confidence=low: converges saved motion, then enables the axe recovery behavior.

#ifdef NONMATCHING
// fuzzy: 81.0%
void SceneActorAnimation_SelectProfileEntry(struct SceneActorAnimationView *, s32);
void Axe_ClearForcedMovement(void *);
void Axe_ResetMovementFlags(void *);
static inline s32 Quarter(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}
static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

void Axe_ConvergeRecoveryMotion(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    s32 x, z;
    s16 *priorX, *priorZ;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        actor->flags2D4 = 0x20 | actor->flags2D4;
        SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        Axe_ClearForcedMovement(actor);
        Axe_ResetMovementFlags(actor);
    }
    {
        s32 current = *(s16 *)((u8 *)actor + 0x310);
        s16 *field = (void *)((u8 *)actor + 0x318);
        s32 difference = current - *field;
        priorX = field;
        x = Quarter(difference);
    }
    {
        s32 current = *(s16 *)((u8 *)actor + 0x314);
        s16 *field = (void *)((u8 *)actor + 0x31C);
        s32 difference = current - *field;
        priorZ = field;
        z = Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->motion.x += x;
    actor->motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        actor->field2B0 = 1;
        actor->flags2D4 &= ~0x20;
        actor->flags2CC &= ~0x80;
        actor->flags2D0 |= 0x100000;
        Axe_ResetMovementFlags(actor);
        *(u32 *)(context + 0xE4C) |= 0x10;
        *(u16 *)((u8 *)actor + 0x418) = *(u16 *)((u8 *)actor + 0x416);
        actor->state2A8 = 0;
        actor->phase2A9 = 2;
        actor->behavior2AA = 9;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Axe_ConvergeRecoveryMotion);
#endif
