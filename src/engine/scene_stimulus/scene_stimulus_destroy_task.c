#include "engine/shared/global.h"
#include "engine/scene_stimulus/scene_stimulus.h"
u32 SceneStimulus_DestroyTask(void)
{
    gSceneStimulusState = 0;
}
