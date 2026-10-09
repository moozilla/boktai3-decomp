#include "global.h"
void sub_08165B7C(void *,int,int);void sub_08166054(void *,int,int);
void sub_081659F4(void *,int);void sub_0816615C(void *);
void sub_08171D08(void);void sub_08163EB8(void *,void (*)(void),int);
void sub_081663A0(void *,int,int,int,int);void sub_08166B7C(void *,int);
void sub_08171274(u8 *p) {
 sub_08165B7C(p,5,1);sub_08165B7C(p,0x3b,1);sub_08166054(*(void **)(p+0x5c),4,0);
 sub_081659F4(p,1);sub_0816615C(p);sub_08163EB8(p,sub_08171D08,1);
 sub_081663A0(p,0,0,1,0);sub_08166B7C(p,3);
}
