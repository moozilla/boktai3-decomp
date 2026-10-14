#include "scene_arc.h"
#define GroundEffect_UpdateProjection sub_0803F650
s32 sub_0811AE68(struct ScenePoolVec32 *);
void GroundEffect_UpdateProjection(struct SceneGroundEffect *effect)
{
    struct SceneVec *screen = &effect->screen;
    struct ScenePoolVec32 position = *effect->position;
    position.y = sub_0811AE68(&position);
    screen->x = (position.x >> 12) - gUnk_020004B8->x;
    screen->z = position.z >> 12;
    screen->y = position.y;
}
