#include "global.h"
void sub_0816616C(void *);void sub_08030F20(s32,s32,s32,s32);void sub_08163F14(void *,u32,s32,s32,u32);void sub_0816446C(void *,s32,s32,s32);void sub_0816471C(void *,s32,s32,s32,u32,u32);void sub_0816568C(void);void sub_08165ACC(void *);void sub_08165B94(void *,s32,s32);void sub_0821980C(void *,void *,u16,s32);void sub_0816618C(void *,s32,s32);void sub_08165B28(void *,s32,s32);void sub_08166CD0(void *);void sub_08166534(void *,s32,s32);void sub_0817080C(void *);void sub_08170718(void *);void sub_081672FC(void *);void sub_0816701C(void *,s32,s32,s32,s32,s32);void sub_08033568(void);void sub_0816D868(void *);void sub_08166094(void *,s32,s32);void sub_08166B7C(void *,s32);void sub_08163EB8(void *,void (*)(void),s32);void sub_0816FCD0(void);
void sub_081708E4(u8 *p) {
 u8 *flag=p+0x4860;u32 z=0;u8 *o;u32 a,b;
 *flag=z;sub_0816616C(p);sub_08030F20(2,16,28,2);
 sub_08163F14(p,*(u32 *)(p+0x18),0,6,*(u32 *)(p+0x2c));sub_0816446C(p,1,2,12);
 o=*(u8 **)(p+0xa40);sub_0816471C(p,1,2,14,*(u16 *)(o+0x428),*(u16 *)(o+0x42a));
 sub_0816568C();sub_08165ACC(p);sub_08165B94(p,0,0x201);
 {u8 *a=p+0x1420;sub_0821980C(a,p+0x64,0x52,0);}sub_0816618C(p,8,32);
 sub_08165B94(p,0x4d,1);sub_08165B94(p,8,1);sub_08165B28(p,8,99);sub_08166CD0(p);sub_08166534(p,10,6);
 sub_0817080C(p);sub_08170718(p);sub_081672FC(p);sub_0816701C(p,0,0,0,1,z);sub_08033568();sub_0816D868(p);sub_08166094(p,0,0);sub_08166B7C(p,4);sub_08163EB8(p,sub_0816FCD0,0);
}
