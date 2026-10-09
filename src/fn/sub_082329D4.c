#include "global.h"
struct S { u8 a,b,c,p3; u32 f4; u8 p8[0x30-8]; s16 h30,h32,h34; };
void sub_08217DE0(void*,void*,u32);
void sub_08217EC4(void*,s32,s32);
void sub_08217EEC(void*,void*,u32);
void sub_082329D4(u8 *a, struct S *s)
{
 u32 zero=0;
 u8 one;
 void *p;
 s->a=zero;
 s->f4=zero;
 one=1;
 s->c=one;
 s->b=zero;
 p=s->p8;
 a+=0x100;
 sub_08217DE0(p,*(void**)a,0x13);
 sub_08217EC4(p,-4,-4);
 sub_08217EEC(p,*(void**)a,0x33);
 ((u8*)s)[0x17]=one;
 s->h30=zero;
 s->h32=zero;
 s->h34=zero;
}
