#include "scene_input_navigation.h"
#define SceneInput_MoveRight sub_0805E940
u32 SceneInput_MoveRight(struct SceneInputGridView *grid){
 u32 original=grid->cursorX;
 s32 base=grid->cells[original*4+grid->cursorY];
 u32 candidate=original+1;
 if(original==8)candidate=0;
 while(candidate!=original){
 s32 value=grid->cells[candidate*4+grid->cursorY];
 if(value>=0 && value!=base){grid->cursorX=candidate;grid->selection=sub_0805E5D0(grid);Sound_PlaySong(0xDC);return 1;}
 if(candidate==8)candidate=0;else candidate++;
 }
 return 0;
}
