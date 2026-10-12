#include "global.h"
#define SceneMessages_IsActive sub_08020F48
extern u8*gUnk_0200047C;
int SceneMessages_IsActive(void){u8*p=gUnk_0200047C;int result;if(!p||p[0x23]!=1)result=0;else result=1;return result;}
