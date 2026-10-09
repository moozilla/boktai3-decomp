#include "global.h"
s32 sub_0822E104(s32);void sub_08170F54(void *);void sub_08030BF8(void);void sub_08033468(void);void sub_0821980C(void *,void *,u16,s32);void sub_08030B78(s32);void sub_08030C84(u32);void sub_08030F20(s32,s32,s32,s32);void sub_08030DBC(s32);void sub_08033568(void);u8 *Script_ParseStringRef(u32);void *Text_LookupString(void *);void sub_0803343C(void *);
void sub_08170F70(u8 *p) {
 s32 r;
 if(p[0x4387]<=15) {r=sub_0822E104(p[0x4387]);if(r==255){r=-1;sub_08170F54(p);}}
 else{sub_08170F54(p);r=-1;}
 {u32 *q=(u32 *)(p+0x2a48);u32 v=*q,m=1;v|=m;*q=v;
 if(r<0){sub_08030BF8();sub_08033468();}
 else {
 v&=~1;*q=v;
 sub_0821980C(p+0x2a40,p+0xa4,0xcf,1);sub_08030B78(1);sub_08030C84(*(u32 *)(p+0x44));
 sub_08030F20(2,16,28,2);sub_08030DBC(r);sub_08033568();
 {u8 *base=Script_ParseStringRef(*(u32 *)(p+0x44))+0x30;u8 **base_pointer=&base;sub_0803343C(Text_LookupString(*base_pointer+r));}
 }
 }
}
