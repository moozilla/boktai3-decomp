#include "scene_input_navigation.h"
u32 SceneInput_MoveLeft(struct SceneInputGridView *grid)
{
    u32 original = grid->cursorX;
    s32 base = grid->cells[original * 4 + grid->cursorY];
    u32 candidate = original - 1;
    if (original == 0)
        candidate = 8;
    while (candidate != original) {
        s32 value = grid->cells[candidate * 4 + grid->cursorY];
        if (value >= 0 && value != base) {
            grid->cursorX = candidate;
            grid->selection = SceneInput_GetGridSelection(grid);
            Sound_PlaySong(0xDC);
            return 1;
        }
        if (candidate == 0)
            candidate = 8;
        else
            candidate--;
    }
    return 0;
}
