#include "global.h"
void sub_08173508(void *,int);
void sub_0816B654(void *,int,int);
void sub_0816D920(void *,int);
void sub_08165B7C(void *,int,int);
void sub_08165B94(void *,int,int);
void sub_0821980C(void *,void *,int,int);
void sub_081735C8(u8 *p) {
 u8 *q=p+0x4384,*r,*v,*a;
 sub_08173508(q,0xe1); sub_0816B654(q,8,0);
 a=p+0x15c0; r=p+0x4387; sub_0816D920(a,*r);
 sub_08165B7C(p,5,1);
 a=p+0x1600; v=p+0x64; sub_0821980C(a,v,0x4d,0);
 sub_08165B7C(p,6,1); sub_0821980C(p+0x1660,v,0x4e,0);
 sub_08165B94(p,4,1); sub_0816D920(p+0x1620,*r);
}
