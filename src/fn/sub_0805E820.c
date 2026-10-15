#include "scene_input_navigation.h"
#define SceneInput_MoveUp sub_0805E820
u32 SceneInput_MoveUp(struct SceneInputGridView *grid)
{
    s32 base = grid->cells[grid->cursorY + grid->cursorX * 4];
    u32 i = (grid->cursorY + 3) & 3;
    while (i != grid->cursorY) {
        s32 v = grid->cells[i + grid->cursorX * 4];
        if (v >= 0 && v != base) {
            grid->cursorY = i;
            grid->selection = sub_0805E5D0(grid);
            Sound_PlaySong(0xdc);
            return 1;
        }
        i = (i + 3) & 3;
    }
    return 0;
}
