#include "ui/widgets/scene_input_navigation.h"
void SceneInput_PositionCursor(struct SceneInputGridView *grid, struct AnimSprite *widget)
{
    SceneInput_GetGridPosition((void *)&widget->position, grid->selection);
}
