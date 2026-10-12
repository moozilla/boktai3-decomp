// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_HasPrimaryId sub_0801E344
int SceneObjects_HasPrimaryId(u8*p,u16 id){int i=0;register int initial asm("r2")=p[0x1c];asm("" : "+r"(initial));if(i<initial){int count=initial;register u16 *ids asm("r2")=(u16*)(p+0x1e);asm("" : "+r"(ids));do{if(*ids==id)return 1;ids++;i++;}while(i<count);}return 0;}
