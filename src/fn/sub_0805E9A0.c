#include "scene_input_navigation.h"
u32 SceneInput_NavigateGrid(struct SceneInputGridView *grid)
{
    u32 keys = SceneInput_GetNavigationKeys(grid);
    switch (keys & 0xC0) {
        case 0x40:
            return SceneInput_MoveUp(grid);
        case 0x80:
            return SceneInput_MoveDown(grid);
        default:
            switch (keys & 0x30) {
                case 0x20:
                    return SceneInput_MoveLeft(grid);
                case 0x10:
                    return SceneInput_MoveRight(grid);
                default:
                    return 0;
            }
    }
}
