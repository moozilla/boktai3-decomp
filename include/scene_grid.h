#ifndef GUARD_SCENE_GRID_H
#define GUARD_SCENE_GRID_H
#include "engine.h"
/* Width is consumed signed by this group of scene-grid helpers. */
static inline s32 SceneGrid_Width(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
#endif
