// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneActors_FindAvailable sub_0802141C
extern u8 *gUnk_020000B8[8];
u8*SceneActors_FindAvailable(void){u8*found=0;u8**cursor;register int i asm("r4")=0;asm("" : "+r"(i));cursor=gUnk_020000B8;do{u8*p;asm("" : "+r"(found));p=*cursor;if(p&&!p[0xb3]){if(found)break;found=p;}cursor++;i++;}while(i<8);return found;}
