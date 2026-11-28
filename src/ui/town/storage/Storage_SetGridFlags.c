#include "global.h"
#include "ui/widgets/scene_input_grid.h"

void Storage_RebuildGrid(struct SceneInputGrid *);

void Storage_SetGridFlags(struct SceneInputGrid *grid, u8 flags)
{
    grid->flags = flags;
    Storage_RebuildGrid(grid);
}
