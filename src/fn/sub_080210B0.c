// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneCommands_FadePalette sub_080210B0
void sub_08003AA8(int,int,void*);
void SceneCommands_FadePalette(void*p,void*unused,u8*message){s16*args=(s16*)(message+8);int out[7],mode;register int mask asm("r3")=0;if(args[5])mask=0xfff;if(args[6]){register int value asm("r1")=0x1000;register int copied asm("r0");asm("" : "+r"(value));copied=value;asm("" : "+r"(copied));mask|=copied;}if(args[7]){register int value asm("r1")=0xe000;register int copied asm("r0");asm("" : "+r"(value));copied=value;asm("" : "+r"(copied));mask|=copied;}out[0]=1;out[1]=args[1];out[2]=args[2];out[3]=args[3];out[4]=args[4];out[5]=mask;out[6]=0;if(!out[2]&&!out[3]&&!out[4]){out[2]=4;out[3]=4;out[4]=4;}{int enabled=args[0];mode=2;if(!enabled)mode=1;}sub_08003AA8(mode,7,out);}
