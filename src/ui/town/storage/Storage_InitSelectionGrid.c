#include "global.h"
#include "ui/widgets/scene_input_navigation.h"
void Storage_SetGridFlags(struct SceneInputGridView *, u32);
void Storage_InitSelectionGrid(struct SceneInputGridView *grid, u32 kind, u32 x, u32 y, u32 flags)
{
    u32 repeat = 0;
    grid->kind = kind;
    Storage_SetGridFlags(grid, flags);
    grid->cursorX = x;
    grid->cursorY = y;
    grid->selection = SceneInput_GetGridSelection(grid);
    grid->repeat = repeat;
}
