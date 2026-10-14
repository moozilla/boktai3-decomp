#include "scene_arc.h"
void GroundEffect_UpdateProjection(struct SceneGroundEffect *);
void GroundEffect_EnableProjection(struct SceneGroundEffect *effect)
{
    effect->flags &= ~1;
    effect->update = GroundEffect_UpdateProjection;
    effect->update(effect);
}
