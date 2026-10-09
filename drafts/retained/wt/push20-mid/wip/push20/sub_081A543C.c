#include "global.h"
void sub_081A5394(s16 *);
s32 sub_08249274(s32,s32);
static inline s32 shift(s32 x,s32 n) {s32 r;if(x>=0)r=x>>n;else r=-((-x)>>n);return r;}
void sub_081A543C(s16 *p)
{
    s16 v[3];
    s32 y=p[1];
    s16 *q=v;
    s32 x=p[0],z=p[2];
    s32 a,b,c;
    q[0]=shift((x-z)*3,5);
    a=shift((x+z)*3,5);
    b=shift(p[1]*3,5);
    q[1]=a-b;
    q[2]=a+b;
    q=v;
    sub_081A5394(v);
    a=v[0];
    b=q[1];
    c=sub_08249274((a+b)<<5,3)+y;
    p[0]=shift(c,1);
    p[1]=y;
    c=sub_08249274((b-a)<<5,3)+y;
    p[2]=shift(c,1);
}
