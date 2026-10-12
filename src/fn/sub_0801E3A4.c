// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObject_CheckActorCompatibility sub_0801E3A4
extern u8 *gUnk_02000580;extern int gUnk_08604D54[][3];
int SceneObject_CheckActorCompatibility(void*unused,u8*p){int i=0,result;register int actor asm("r4")=gUnk_02000580[0x3a4];register int *ids asm("r1");register int *table asm("r2")=&gUnk_08604D54[0][0];asm("" : "+r"(actor),"+r"(table));{register int offset asm("r0")=p[0x38]*12;asm("" : "+r"(offset));ids=(int*)(offset+(int)table);}scan:if(actor!=*ids)goto next;result=0;goto end;next:ids++;i++;if(i<=2)goto scan;result=1;end:return result;}
