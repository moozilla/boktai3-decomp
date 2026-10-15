#include "scene_input_navigation.h"
#define SceneInput_NavigateGrid sub_0805E9A0
u32 SceneInput_NavigateGrid(struct SceneInputGridView *grid){
 u32 keys=sub_0805E7D8(grid);
 switch(keys&0xC0){
 case 0x40:return sub_0805E820(grid);
 case 0x80:return sub_0805E880(grid);
 default:switch(keys&0x30){
 case 0x20:return sub_0805E8E0(grid);
 case 0x10:return sub_0805E940(grid);
 default:return 0;
 }
 }
}
