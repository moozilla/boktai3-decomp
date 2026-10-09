#include "global.h"
u32 Script_SeekToKeyword(u32);void *Script_GetPc(void);s32 Script_GetValue(void);
u32 sub_0821ABA8(u32,u32);void sub_0822B6C8(s16,s16,s16,s16,u8,u16);
void sub_0822B968(void) {
 u32 a,b,c,zero;
 if(Script_SeekToKeyword('p')) {
 a=Script_GetPc()?(u16)Script_GetValue():0;
 b=Script_GetPc()?(u16)Script_GetValue():0;
 }else {a=0;b=0;}
 c=(u16)sub_0821ABA8('r',0);
 sub_0822B6C8((s16)a,(s16)b,(s16)c,zero=0,(u8)sub_0821ABA8('f',1),zero);
}
