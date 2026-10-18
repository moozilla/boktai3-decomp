#define SceneInput_MoveUp    Hud_MoveGridUp
#define SceneInput_MoveDown  Hud_MoveGridDown
#define SceneInput_MoveLeft  Hud_MoveGridLeft
#define SceneInput_MoveRight Hud_MoveGridRight
#include "global.h"
#include "scene_input_navigation.h"
u32 SceneInput_NavigateGridKeys(struct SceneInputGridView *grid, u32 keys)
{
    u32 allowed = SceneInput_ReadRepeatingDirections(grid);
    if ((allowed & 0x40) && (keys & 0x40))
        return SceneInput_MoveUp(grid);
    if ((allowed & 0x80) && (keys & 0x80))
        return SceneInput_MoveDown(grid);
    if ((allowed & 0x20) && (keys & 0x20))
        return SceneInput_MoveLeft(grid);
    if ((allowed & 0x10) && (keys & 0x10))
        return SceneInput_MoveRight(grid);
    return 0;
}
