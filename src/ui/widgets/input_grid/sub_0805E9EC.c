#include "ui/widgets/scene_input_navigation.h"
void SceneInput_PositionCursor(struct SceneInputGridView *grid, struct AnimSprite *animSprite)
{
    SceneInput_GetGridPosition((void *)&animSprite->position, grid->selection);
}
