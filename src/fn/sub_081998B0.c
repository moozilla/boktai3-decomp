#include "global.h"
#include "ram.h"
#include "game_state.h"
#include "scene_actor_registry.h"
#include "scene_effect_projection.h"

#define SceneEffectQueue_QueueTypeOneEffect sub_081998B0
s32 SceneEffectQueue_QueueTypeOneEffect(void)
{
    union SceneActorRegistryPackedPosition in;
    u8 *g;
    s32 next, left, stride;
    u32 one;
    if (!gSceneEffectQueue || (gEnginePauseFlags & 7))
        goto fail;
    in.packed.x = (u16)gGameState->positionX;
    in.packed.y = (u16)gGameState->positionY;
    in.packed.z = (u16)gGameState->positionZ;
    SET_PROJECTED_EFFECT_SIDE(&in.position, left);
    g = gSceneEffectQueue;
    one = 1;
    next = g[0x1e];
    next++;
    if (next > 1)
        next -= 2;
    if (next != g[0x1f])
        goto write;
fail:
    return 0;
write: {
    u8 *base = g;
    u8 **bp = &base;
    *(*bp + g[0x1e] * (stride = 4) + 0x2c) = one;
    *(*bp + g[0x1e] * stride + 0x2d) = left;
    *(*bp + g[0x1e] * stride + 0x2e) = one;
    g[0x1e] = next;
    return 1;
}
}
