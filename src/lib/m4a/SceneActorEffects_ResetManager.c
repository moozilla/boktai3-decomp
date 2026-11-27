// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* SceneActorEffects_ResetManager: Clears the singleton manager pointer shared by actor effect groups */

void SceneActorEffects_ResetManager(void)
{
    gSceneActorEffectsManager = 0;
}
