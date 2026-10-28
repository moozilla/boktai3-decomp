#include "scene_arc.h"
s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);
void GroundEffect_UpdateProjection(struct SceneGroundEffect *effect)
{
    struct SceneVec *screen = &effect->screen;
    struct ScenePoolVec32 position = *effect->position;
    position.y = Terrain_GetHeightAtFixedPosition(&position);
    screen->x = (position.x >> 12) - gSceneCamera->x;
    screen->z = position.z >> 12;
    screen->y = position.y;
}
