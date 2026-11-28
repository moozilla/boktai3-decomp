#include "global.h"
#include "engine/video/render.h"
#include "bosses/count/scene_graphics_actor.h"

struct SceneGraphicsActor *SceneActors_GetBySlot(int);
int Script_GetKeywordValue(int, int);

void Count_QueueDefaultCommand(void)
{
    struct SceneGraphicsActor *actor = SceneActors_GetBySlot(0);
    if (actor && actor->graphics.backgroundFlash) {
        int stop = Script_GetKeywordValue('s', 0);
        int alternate = Script_GetKeywordValue('h', 0);
        if (!actor->graphics.flashPhase) {
            if (stop)
                actor->graphics.flashPhase = 3;
            else if (alternate)
                actor->graphics.flashPhase = 1;
            else
                actor->graphics.flashPhase = 2;
            actor->graphics.initialFlashFlag = 0;
        }
    }
}
