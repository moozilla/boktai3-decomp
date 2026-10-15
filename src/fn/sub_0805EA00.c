#include "scene_input_navigation.h"
#define SceneInput_SaveSelection sub_0805EA00
void SceneInput_SaveSelection(struct SceneInputGridView *grid){grid->savedX=grid->cursorX;grid->savedY=grid->cursorY;grid->savedSelection=grid->selection;}
