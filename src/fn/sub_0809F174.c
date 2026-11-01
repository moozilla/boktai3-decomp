#include "scene_actor_chase.h"
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"

void Crow_DisableAttackCollision(struct SceneActorRegistryResetView *);
static inline void OrFlags(u16 *field, u32 mask)
{
    u32 value = *field;
    mask = value | mask;
    *field = mask;
}
static inline s32 Quarter(s32 n)
{
    if (n >= 0)
        n = n >> 2;
    else
        n = -((-n) >> 2);
    return n;
}
static inline s32 Clamp(s32 n)
{
    if (n > 200)
        n = 200;
    else if (n < -200)
        n = -200;
    return n;
}
static inline u8 EaseMotion(struct SceneActorRegistryResetView *actor)
{
    s32 x = Quarter(*(s16 *)((u8 *)actor + 0x310) - *(s16 *)((u8 *)actor + 0x318));
    s32 z = Quarter(*(s16 *)((u8 *)actor + 0x314) - *(s16 *)((u8 *)actor + 0x31C));
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *(s16 *)((u8 *)actor + 0x318) += x;
    *(s16 *)((u8 *)actor + 0x31C) += z;
    if (x == 0 && z == 0)
        return 1;
    return 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u8 behavior, u32 zw)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = zw;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind,
                              u32 zb, u32 zw)
{
    actor->field2B2 = 1;
    actor->field2B0 = zb;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = zw;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zw;
}
// confidence=low: eases the stored reaction displacement to its target before restoring idle.
void Crow_RecoverFromDisplacement(struct SceneActorRegistryResetView *actor, s32 elapsed)
{
    u8 *state = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u32 m = 0x20;
        m |= actor->flags2D4;
        actor->flags2D4 = m;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    }
    {
        u32 mask = 0x100000;
        actor->flags2D0 |= mask;
    }
    if (elapsed < 20) {
        u32 m = 0x20;
        m |= actor->flags2D6;
        actor->flags2D6 = m;
    }
    if (EaseMotion(actor) && elapsed > 30) {
        u32 clearedWord;
        u16 v = *(u16 *)((u8 *)actor + 0x178) + 10;
        u16 initialState[] = {0};
        clearedWord = 0;

        *(u16 *)((u8 *)actor + 0x178) = v;
        actor->field2B0 = 1;
        {
            u16 *field = &actor->flags2D4;
            u32 mask;
            mask = ~0x20;
            ((struct ChaseFlags40 *)field)->bits &= mask;
        }
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x80;
            *flags &= mask;
        }
        state[0x3EC] = initialState[0];
        Crow_DisableAttackCollision(actor);
        actor->body.direction = actor->tracking.heading;
        SetControl(actor, SceneActorControl_OnIdle, 16, initialState[0], clearedWord);
        SetState(actor, initialState[0], initialState[0], 1, clearedWord);
        actor->body.direction = actor->tracking.heading;
        actor->field2C2 = 64;
    }
}
