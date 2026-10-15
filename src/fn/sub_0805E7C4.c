#include "scene_input_grid.h"
#define SceneInput_SetGridFlags sub_0805E7C4
void SceneInput_SetGridFlags(struct SceneInputGrid *grid, u32 flags)
{
    grid->flags = flags;
    sub_0805E5E4(grid);
}
