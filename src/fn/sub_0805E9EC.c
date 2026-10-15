#include "scene_input_navigation.h"
void SceneInput_PositionCursor(struct SceneInputGridView *grid, struct SceneInputWidget *widget)
{
    SceneInput_GetGridPosition((void *)&widget->position, grid->selection);
}
