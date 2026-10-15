#include "scene_input_navigation.h"
#define SceneInput_InitGrid sub_0805EA20
void SceneInput_InitGrid(struct SceneInputGridView *grid,u32 kind,u32 x,u32 y,u32 flags){u32 zero=0;grid->kind=kind;sub_0805E7C4(grid,flags);grid->cursorX=x;grid->cursorY=y;grid->selection=sub_0805E5D0(grid);grid->repeat=zero;}
