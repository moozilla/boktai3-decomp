#include "global.h"
extern u8 *gUnk_02000274,*gUnk_02000480;
s32 Script_SeekToKeyword(s32);s32 sub_0821ABA8(s32,s32);u8 *sub_0802D6D0(void *,s32);void sub_081CEFCC(void);
void sub_081D10D8(void) {
 u8 *p;
 if(!gUnk_02000274)return;
 if(Script_SeekToKeyword(0x6e)) {
 p=gUnk_02000480;if(!p)return;
 p=sub_0802D6D0(p,sub_0821ABA8(0x6e,0));if(!p)return;
 }else p=gUnk_02000274;
 {u8 *q=p+0x21b;s32 zero=0,one=1;void (*f)(void);*q=one;{s32 kind=0x12;p[2]=zero;p[3]=zero;p[4]=kind;}*(u32 *)(p+0x14)=zero;p[8]=one;f=sub_081CEFCC;p[6]=0x10;
 *(void (**)(void))(p+0x2cc)=f;*(u32 *)(p+0x10)=zero;p[9]=one;p[0x18]=zero;}
}
