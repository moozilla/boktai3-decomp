// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_motion.h"
#include "player.h"
static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}
static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 LiftFlag(struct SceneActorControlMotionState *state)
{
    u32 mask = 2;
    u32 offset;
    offset = 0x3D4;
    return Test(*(u32 *)((u8 *)state + offset), mask);
}
void SceneActorControl_ConsumeLiftFlag(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state;
    if (IsUnavailable(actor))
        return;
    if (!gPlayers[0])
        return;
    state = SceneActorControl_GetMotionState(actor);
    if (LiftFlag(state)) {
        u32 *flags = &state->flags3D4;
        u32 clearMask;
        u8 *parameters;
        register u32 bit asm("r0");
        register u32 old asm("r1");
        u32 wordZero;
        u16 *actorFlags;
        u8 byteZero;
        u8 behavior;
        clearMask = ~2;
        *flags &= clearMask;
        parameters = (u8 *)actor + 0xDC;
        bit = 8;
        old = parameters[0xF];
        wordZero = 0;
        parameters[0xF] = bit | old;
        bit = 0x100;
        actorFlags = &actor->flags2D4;
        old = *actorFlags;
        byteZero = 0;
        /* Keep the initialized byte zero distinct from the word counter clear. */
        asm("" : "+r"(byteZero) : "r"(old));
        *actorFlags = bit | old;
        behavior = 3;
        actor->state2A8 = byteZero;
        actor->phase2A9 = byteZero;
        actor->behavior2AA = behavior;
        actor->counter2AC = wordZero;
        actor->field2B1 = 1;
    }
}
