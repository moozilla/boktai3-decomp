#include "scene_input_navigation.h"
void SceneInput_InitGrid(struct SceneInputGridView *grid, u32 kind, u32 x, u32 y, u32 flags)
{
    u32 zero = 0;
    grid->kind = kind;
    SceneInput_SetGridFlags(grid, flags);
    grid->cursorX = x;
    grid->cursorY = y;
    grid->selection = SceneInput_GetGridSelection(grid);
    grid->repeat = zero;
}
