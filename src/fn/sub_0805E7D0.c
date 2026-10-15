#include "scene_input_grid.h"
u32 SceneInput_TestGridFlags(struct SceneInputGrid *grid, u32 mask)
{
    return grid->flags & mask;
}
