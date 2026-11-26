#include "global.h"
#include "bosses/nidhoggr/nidhoggr_effect_pool.h"
void SceneEffect_ResetState(struct ScenePanelOwnedEffect *effect)
{
    effect->anchor = NULL;
    ZeroFill8(&effect->motion);
    effect->state = 0;
    effect->age = 0;
    effect->referenceCoordinate = 0;
    effect->owner = NULL;
}
