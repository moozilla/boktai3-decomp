#include "scene_arc.h"
void GroundEffect_IgnoreUpdate(struct SceneGroundEffect *);
void GroundEffect_DisableProjection(struct SceneGroundEffect *effect)
{
    effect->flags |= 1;
    effect->update = GroundEffect_IgnoreUpdate;
}
