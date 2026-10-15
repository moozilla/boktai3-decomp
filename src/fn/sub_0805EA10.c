#include "scene_input_navigation.h"
void SceneInput_RestoreSelection(struct SceneInputGridView *grid)
{
    grid->cursorX = grid->savedX;
    grid->cursorY = grid->savedY;
    grid->selection = grid->savedSelection;
}
