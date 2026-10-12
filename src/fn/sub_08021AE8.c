// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneActors_RunFirstEventScript sub_08021AE8
extern u32 gUnk_02000560;void sub_0821AD08(int,void*);
void SceneActors_RunFirstEventScript(u8*p,s16*position){int args[8];struct {u32 count:16;u32 unused:16;void*args;}call;register int script asm("r0")=*(int*)(p+0xe8);asm("" : "+r"(script));if(script){int id=script;int mask=4;asm("" : "+r"(mask));if(!(gUnk_02000560&mask)){int *in,*out,i;args[0]=position[0];args[1]=position[1];args[2]=position[2];{register void *cp asm("r4")=&call;asm("" : : "r"(cp));}in=(int*)(p+0xf0);out=args+3;i=3;do {*out++=*in++;i--;}while(i>=0);{register u32 mask asm("r0")=0xffff0000;register u32 header asm("r1");asm("" : "+r"(mask));header=*(u32*)&call;header&=mask;asm("" : "+r"(header));header|=7;asm("" : "+r"(header));*(u32*)&call=header;}call.args=args;sub_0821AD08(id,&call);}}}

