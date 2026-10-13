#include "global.h"
#define SceneObjects_SetInactive sub_0801E698
void sub_0801EF58(void);
void SceneObjects_SetInactive(u8*p,u16 id){int i=0;if(i<*(u16*)(p+0x1a)){u8 **base=(u8**)(p+0x42c);int two;int zero;void *fn;zero=0;fn=sub_0801EF58;two=2;do{u8*s=*base+140*i;if(*(u16*)(s+0x34)==id){s[0x37]=zero;*(void**)(s+0x30)=fn;s[0x39]=two;s[0x3a]=zero;}i++;}while(i<*(u16*)(p+0x1a));}}
