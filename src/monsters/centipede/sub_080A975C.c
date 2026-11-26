#include "monsters/centipede/centipede_body.h"
#include "scene/actor/scene_actor_control_helpers.h"

u32 TerrainOverlay_QueueRemoveActor(u32 *p);
void SceneBody_Unregister(void *);

static inline u8 TestMask(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}

void SpritePool_ConsumeStateFlag(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct TentacleBody *state = (void *)SceneActorControl_GetState(actor);
    u32 mask = 1;
    if (TestMask(&state->attackFlags, mask)) {
        u16 *flagsPtr = &state->attackFlags;
        u32 clearMask = ~1;
        *flagsPtr &= clearMask;
        {
            void *obj = &state->markedSegmentBody;
            TerrainOverlay_QueueRemoveActor(obj);
            SceneBody_Unregister(obj);
        }
    }
}
