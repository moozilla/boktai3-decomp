#include "global.h"
#define SceneObjects_SetActive sub_0801E630
void sub_0801EF5C(void);
void SceneObjects_SetActive(u8*p,u16 id){int i=0;if(i<*(u16*)(p+0x1a)){u8 **base=(u8**)(p+0x42c);int one;int zero;one=1;zero=0;do{u8*s=*base+140*i;if(*(u16*)(s+0x34)==id){s[0x37]=one;*(void**)(s+0x30)=sub_0801EF5C;s[0x39]=zero;s[0x3a]=zero;}i++;}while(i<*(u16*)(p+0x1a));}}
