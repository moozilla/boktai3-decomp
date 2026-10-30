#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_script.h"
static inline u8 ContextHasFlag(struct SceneActorScriptPropertiesView *p, u32 mask)
{
    if (p->flags6DC & mask)
        return 1;
    return 0;
}
u8 SceneActorReaction_IsRangeReady(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorScriptPropertiesView *context = actor->context;
    switch (actor->paletteOffset) {
        case 0:
        case 5:
        case 12:
            if (actor->category == 5) {
                if (context->timer6F0 <= 0)
                    return 1;
            } else if (actor->category == 7) {
                if (actor->health <
                        ((struct SceneActorReactionHealthTail *)context)->healthThreshold ||
                    ContextHasFlag(context, 0x80)) {
                    if (context->timer6F0 <= 0)
                        return 1;
                } else {
                    if (context->timer6F2 <= 0)
                        return 1;
                }
            } else {
                if (actor->tracking.distance < context->distanceThreshold6E0 && context->timer6F0 <= 0)
                    return 1;
                if (actor->tracking.distance < context->distanceThreshold6E4 && context->timer6F2 <= 0)
                    return 1;
                if (actor->tracking.distance < context->distanceThreshold6E8 && context->timer6F4 <= 0)
                    return 1;
            }
            break;
    }
    return 0;
}
