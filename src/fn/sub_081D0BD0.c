#include "global.h"
struct S {u8 pad[7];u8 b;u8 gap[6];u16 h;u32 n;u8 gap2[0x2cc - 0x14];void (*fn)(void *,void *,u32);};
u32 sub_081D0BD0(void *a,struct S *p) {
 u32 n;
 void (*fn)(void *,void *,u32);
 p->h=0; p->b=0;
 n=p->n++;
 fn=p->fn;
 if(fn) fn(a,p,n);
 return 0;
}
