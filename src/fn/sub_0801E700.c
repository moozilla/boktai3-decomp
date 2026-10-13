#include "global.h"
#define SceneObjects_Lock sub_0801E700
void SceneObjects_Lock(u8*p,u16 id){int i=0;if(i<*(u16*)(p+0x1a)){u8**base=(u8**)(p+0x42c);int one;one=1;do{u8*s=*base+140*i;if(*(u16*)(s+0x34)==id)s[0x36]|=1;i++;}while(i<*(u16*)(p+0x1a));}}
