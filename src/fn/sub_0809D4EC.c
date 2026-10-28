#include "global.h"
#include "scene_actor_control_helpers.h"
#include "player.h"

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline u8 LiftFlag(struct SceneActorControlMotionState *state)
{
    u32 mask = 2;
    u32 offset = 0x3D4;
    return Test(*(u32 *)((u8 *)state + offset), mask);
}

void SceneActorControl_ConsumeLiftFlagPenalty(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    if (!LiftFlag((void *)SceneActorControl_GetMotionState(actor)))
        return;
    if (actor->paletteOffset == 0) {
        u8 *p = (*(u8 **)gPlayers) + 0x4F6;
        *p = *p - 1;
    }
    {
        u32 *flags = (void *)((u8 *)SceneActorControl_GetMotionState(actor) + 0x3D4);
        u32 clearMask = ~2;
        *flags &= clearMask;
    }
}
