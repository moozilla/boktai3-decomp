#include "global.h"
void sub_08170D78(void *);void sub_0816B644(void *);void sub_08170B48(void *,int);
void sub_08165B94(void *,int,int);void sub_08165B7C(void *,int,int);
void sub_08166CD0(void *);void sub_08166534(void *,int,int);
void sub_08166ED4(void *,int,int,int,int);void sub_08166F84(void *,int,int,int,int);
void sub_081672FC(void *);void sub_08171524(void);void sub_08163EB8(void *,void (*)(void),int);
void sub_08030BF8(void);void sub_081663A0(void *,int,int,int,int);
void sub_08170F54(void *);void sub_08166B7C(void *,int);
void sub_0817147C(u8 *p) {
 u8 *q;int z;
 sub_08170D78(p);q=p+0x4384;sub_0816B644(q);sub_08170B48(q,0xe1);
 sub_08165B94(p,4,1);sub_08165B7C(p,6,1);sub_08165B7C(p,5,1);
 sub_08166CD0(p);sub_08166534(p,10,6);z=0;
 sub_08166ED4(p,8,3,0,z);sub_08166F84(p,8,4,0,z);sub_081672FC(p);
 sub_08163EB8(p,sub_08171524,0);sub_08030BF8();sub_081663A0(p,0,1,0,z);
 sub_08170F54(p);sub_08166B7C(p,3);
}
