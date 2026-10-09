#include "global.h"
void sub_08210D60(void*);void sub_08210F04(void*);
s32 Script_SeekToKeyword(s32);
void *sub_08227E90(void);void *sub_0821ABA8(s32,s32);
void sub_08210AD4(void*);void sub_08210B50(void*);void sub_08210BEC(void*);
void *sub_08033690(u32,u32,u32,u32);
void sub_08033568(void);void sub_08033468(void);u32 sub_08210CB0(void*);
void sub_082279A8(s32,s32,s32,s32,s32,s32,s32);
void sub_0822B2F8(u32);
s32 sub_0821159C(u8 *s)
{
 u32 zero=0,four;
 s16 *p;
 void *v;
 *(u32*)(s+0x20)=zero;
 *(u16*)(s+0xb80)=zero;
 *(u16*)(s+0xb82)=zero;
 p=(s16*)(s+0xb8e);four=4;*p=four;
 *(u16*)(s+0xb8c)=zero;
 *(u16*)(s+0xb88)=zero;
 sub_08210D60(s);sub_08210F04(s);
 if(!Script_SeekToKeyword(0x6d)) goto fail;
 if(!(*(void**)(s+0xb9c)=sub_08227E90())) goto fail;
 if(!Script_SeekToKeyword(0x6e)) goto fail;
 if(!(*(void**)(s+0xb98)=sub_08227E90())) goto fail;
 if(!Script_SeekToKeyword(0x68)) goto fail;
 if((*(void**)(s+0xba0)=sub_08227E90())) goto success;
 fail: return -1;
 success:
 *(void**)(s+0xba4)=sub_0821ABA8(0x65,0);
 sub_08210AD4(s);sub_08210B50(s);sub_08210BEC(s);
 *(void**)(s+0xb94)=sub_08033690(1,0x10,0x1d,4);
 sub_08033568();sub_08033468();sub_08210CB0(s);
 sub_082279A8(0,*p,4,4,four,0xffff,zero);
 sub_0822B2F8(0x118);
 return 0;
}
