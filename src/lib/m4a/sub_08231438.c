// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* SceneActorEffects_ResetManager: Clears the singleton manager pointer shared by actor effect groups */
#define SceneActorEffects_ResetManager sub_08231438

void sub_08231438(void)
{
    gSceneActorEffectsManager = 0;
}
