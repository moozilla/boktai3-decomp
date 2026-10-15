#include "scene_input_grid.h"
/* The navigation helpers access all nine contiguous columns. */
struct SceneInputGridSelectionView {
    u8 kind, cursorX, cursorY;
    u8 unknown3[9];
    s8 cells[9][4];
};
#define SceneInput_GetGridSelection sub_0805E5D0
s8 SceneInput_GetGridSelection(struct SceneInputGrid *owner)
{
    struct SceneInputGridSelectionView *grid = (void *)owner;
    return grid->cells[grid->cursorX][grid->cursorY];
}
