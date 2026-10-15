// MATCHING_CONCESSION: register-pinning
#include "scene_input_navigation.h"
#define SceneInput_GetNavigationKeys sub_0805E7D8
u32 SceneInput_GetNavigationKeys(struct SceneInputGridView *grid){
 const volatile u16 *keys=gUnk_03005260;
 u32 value=keys[1];u32 mask=0xF0;u32 pressed;asm("" : "+r"(mask));pressed=mask;
 pressed&=value;
 if(pressed){grid->repeat=0;return keys[1];}
 else{
 mask&=keys[0];
 if(mask){grid->repeat++;if(grid->repeat>9){grid->repeat=pressed;return keys[0];}}
 else grid->repeat=mask;
 return 0;
 }
}
