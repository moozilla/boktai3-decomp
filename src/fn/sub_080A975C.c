#include "global.h"
#include "scene_actor_control_helpers.h"

#define SpritePool_ConsumeStateFlag sub_080A975C

u32 TerrainOverlay_QueueRemoveActor(u32 *p);
void SceneBody_Unregister(void *);

struct State080A9 {
    u8 pad0[0x888];
    u8 obj[0x8d0 - 0x888];
    u16 state;
};

static inline u8 TestMask(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

void sub_080A975C(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct State080A9 *state = (void *)SceneActorControl_GetState(actor);
    u32 mask = 1;
    if (TestMask(&state->state, mask)) {
        u16 *flagsPtr = &state->state;
        u32 clearMask = ~1;
        *flagsPtr &= clearMask;
        {
            void *obj = state->obj;
            TerrainOverlay_QueueRemoveActor(obj);
            SceneBody_Unregister(obj);
        }
    }
}
