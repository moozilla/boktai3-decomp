#include "scene_input_navigation.h"
#define SceneInput_RestoreSelection sub_0805EA10
void SceneInput_RestoreSelection(struct SceneInputGridView *grid){grid->cursorX=grid->savedX;grid->cursorY=grid->savedY;grid->selection=grid->savedSelection;}
