#include "scene_input_navigation.h"
u32 SceneInput_MoveDown(struct SceneInputGridView *grid)
{
    s32 base = grid->cells[grid->cursorY + grid->cursorX * 4];
    u32 i = (grid->cursorY + 1) & 3;
    while (i != grid->cursorY) {
        s32 v = grid->cells[i + grid->cursorX * 4];
        if (v >= 0 && v != base) {
            grid->cursorY = i;
            grid->selection = SceneInput_GetGridSelection(grid);
            Sound_PlaySong(0xdc);
            return 1;
        }
        i = (i + 1) & 3;
    }
    return 0;
}
