#include "global.h"
extern u16 gUnk_03005260[];
void sub_0816618C(void *,s32,s32);void sub_0822B2F8(s32);void sub_0816B634(void *);void sub_0816D920(void *,s32);
void sub_081738D0(void *);void sub_08173928(void *,s32);void sub_081739A8(void *,s32);void sub_08173A14(void *,s32);
void sub_08165B94(void *,s32,s32);void sub_081708CC(void *);s32 sub_0816B59C(void *,s32);void sub_08173470(void *);void sub_08166B7C(void *,s32);
void sub_08173B24(u8 *p) {
 sub_0816618C(p,8,32);p[0xa58]=0;
 if(1&gUnk_03005260[1]) {
 u8 *q;
 sub_0822B2F8(0xdd);sub_0816B634(p+0x4384);
 {u8 *a=p+0x15c0;q=p+0x4387;sub_0816D920(a,*q);}
 if(*q==0x27){sub_081738D0(p);return;}
 if(*q==0x20)sub_08173928(p,1);
 else if(*q==0x26)sub_081739A8(p,1);
 else if(*q==0x28)sub_08173A14(p,1);
 sub_08165B94(p,5,1);sub_081708CC(p);sub_0816D920(p+0x1620,p[0x4387]);
 }else if(sub_0816B59C(p+0x4384,0xc0)) {
 sub_0816D920(p+0x15c0,p[0x4387]);sub_08173470(p);sub_08166B7C(p,2);
 }
}
