#include "global.h"
void sub_0816616C(void *);void sub_08163F14(void *,u32,int,int,u32);
void sub_08165ACC(void *);void sub_08165B28(void *,int,int);void sub_08165B94(void *,int,int);
void sub_0816618C(void *,int,int);void sub_08172A70(void *);void sub_0816568C(void);
void sub_08172950(void *);void sub_081724D4(void *);void sub_08166094(void *,int,int);
void sub_08166B7C(void *,int);void sub_081731E8(void);void sub_08163EB8(void *,void (*)(void),int);
void sub_081663A0(void *,int,int,int,int);void sub_080335B4(void);void sub_08166CD0(void *);
void sub_08166ED4(void *,int,int,int,int);
void sub_08173390(u8 *p) {
 int z;
 sub_0816616C(p);sub_08163F14(p,*(u32 *)(p+0x18),0,7,*(u32 *)(p+0x2c));
 sub_08165ACC(p);sub_08165B28(p,0,0x54);sub_08165B94(p,0x4d,1);sub_0816618C(p,8,0x20);
 sub_08165B28(p,8,0x71);sub_08172A70(p);sub_0816568C();sub_08172950(p);sub_081724D4(p);
 sub_08166094(p,0,0);{u32 v=p[0x4387];int b=1;if(v<=7)b=0;sub_08166B7C(p,b);}
 sub_08163EB8(p,sub_081731E8,0);z=0;sub_081663A0(p,0,1,0,z);
 sub_080335B4();sub_08166CD0(p);sub_08166ED4(p,3,14,0,z);
}
