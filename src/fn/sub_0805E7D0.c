#include "scene_input_grid.h"
#define SceneInput_TestGridFlags sub_0805E7D0
u32 SceneInput_TestGridFlags(struct SceneInputGrid *grid, u32 mask)
{
    return grid->flags & mask;
}
