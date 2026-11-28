#include "global.h"
#include "engine/video/render.h"
#include "bosses/count/scene_graphics_actor.h"

struct SceneGraphicsActor *SceneActors_GetBySlot(int);
int Script_GetKeywordValue(int, int);
void SceneActors_StopBackgroundFlash(void *);
void BackgroundFlash_BeginAlternating(void *);
void BackgroundFlash_BeginAppearance(void *);

void Count_TriggerBackgroundFlash(void)
{
    struct SceneGraphicsActor *actor = SceneActors_GetBySlot(0);
    if (actor && actor->graphics.backgroundFlash) {
        int stop = Script_GetKeywordValue('s', 0);
        int alternate = Script_GetKeywordValue('h', 0);
        if (stop)
            SceneActors_StopBackgroundFlash(actor);
        else if (alternate)
            BackgroundFlash_BeginAlternating(actor->graphics.backgroundFlash);
        else
            BackgroundFlash_BeginAppearance(actor->graphics.backgroundFlash);
    }
}
