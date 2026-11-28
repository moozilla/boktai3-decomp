#include "global.h"
#include "engine/video/render.h"
#include "bosses/count/scene_graphics_actor.h"

struct SceneGraphicsActor *SceneActors_GetBySlot(int);

int Count_TryQueueCommand(int value)
{
    struct SceneGraphicsActor *actor = SceneActors_GetBySlot(0);
    if (actor && actor->graphics.backgroundFlash && !actor->graphics.flashPhase &&
        !(gEnginePauseFlags & 7)) {
        actor->graphics.flashPhase = 1;
        actor->graphics.initialFlashFlag = value;
        return 1;
    }
    return 0;
}
