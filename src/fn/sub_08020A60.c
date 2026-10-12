// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneMessages_RunScript sub_08020A60
int sub_08020D88(int);void sub_0821AD08(int,void*);void sub_08020D68(void*,int);
void SceneMessages_RunScript(void*p,void*entry,u8*message){int args[8];struct{u32 count:16;u32 unused:16;void*args;}call;int id=(u16)sub_08020D88(*(u16*)(message+8));if(id){register int count asm("r0")=message[7];{register void *cp asm("r5")=&call;asm("" : "+r"(count) : "r"(cp));}if(count>1){s16 *in=(s16*)(message+0xa);register int *out asm("r3")=args;int remaining=count-1;asm("" : : "r"(out));do{*out++=*in++;remaining--;}while(remaining);}{register u32 mask asm("r0")=0xffff0000;register u32 header asm("r1");asm("" : "+r"(mask));header=*(u32*)&call;header&=mask;asm("" : "+r"(header));header|=8;asm("" : "+r"(header));*(u32*)&call=header;}call.args=args;sub_0821AD08(id,&call);}sub_08020D68(entry,1);}
