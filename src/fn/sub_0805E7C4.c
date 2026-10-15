#include "scene_input_grid.h"
void SceneInput_SetGridFlags(struct SceneInputGrid *grid, u32 flags)
{
    grid->flags = flags;
    SceneInput_RebuildGrid(grid);
}
