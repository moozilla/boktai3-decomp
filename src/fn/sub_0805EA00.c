#include "scene_input_navigation.h"
void SceneInput_SaveSelection(struct SceneInputGridView *grid)
{
    grid->savedX = grid->cursorX;
    grid->savedY = grid->cursorY;
    grid->savedSelection = grid->selection;
}
