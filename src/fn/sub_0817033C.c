#include "global.h"
extern u16 gUnk_03005260[];
s32 sub_0816B59C(void *,s32);void sub_0816D920(void *,s32);void sub_0816D868(void *);void sub_08166B7C(void *,s32);void sub_0816B614(void *);s32 sub_0816CEB4(s32,s32);void sub_0822B2F8(s32);void sub_08165B7C(void *,s32,s32);void sub_08166054(u32,s32,s32);void sub_081659F4(void *,s32);void sub_081663A0(void *,s32,s32,s32,s32);void sub_08163EB8(void *,void (*)(void),s32);void sub_0817014C(void);void sub_0816DAF8(void *);
void sub_0817033C(u8 *p) {
 u8 *q=p+0x4384;
 if(sub_0816B59C(q,0xf0)){sub_0816D920(p+0x1620,p[0x4387]);sub_0816D868(p);}
 sub_08166B7C(p,4);
 if(1&gUnk_03005260[1]) {
 sub_0816B614(q);
 if(sub_0816CEB4(p[0x4860],p[0x438a])<0)sub_0822B2F8(0x192);
 else {
 sub_08165B7C(p,0x3b,1);sub_08166054(*(u32 *)(p+0x5c),2,1);
 sub_081659F4(p,0);sub_081663A0(p,0,0,1,0);sub_08163EB8(p,sub_0817014C,1);
 }
 }else if(2&gUnk_03005260[1]){sub_0822B2F8(0xde);sub_0816DAF8(p);}
}
