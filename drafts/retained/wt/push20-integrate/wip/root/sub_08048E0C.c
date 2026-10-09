#include "global.h"
extern u8 *gUnk_030042E4;
struct RGB48 {u16 r:5;u16 g:5;u16 b:5;};
struct S48E {u8 pad0[0x24];u16 colors[16];u8 pad1[0x37c-0x44];u16 index;u8 pad2[0x394-0x37e];u16 blend;};
void sub_08048E0C(struct S48E *s)
{
 u16 *a=(u16 *)(gUnk_030042E4+(s->index+0x283)*32);
 u16 *b=(u16 *)(gUnk_030042E4+0x5160);
 s32 inv=64-s->blend;
 s32 weight=s->blend;
 s32 i=0;
 do {
  struct RGB48 *c=(struct RGB48 *)a,*d=(struct RGB48 *)b;
  s32 r=c->r,g=c->g,bl=c->b;
  s32 rr=d->r,gg=d->g,bb=d->b;
  s32 red=((inv*r+weight*rr)>>6)&31;
  s32 green=((inv*g+weight*gg)>>6)&31;
  s32 blue=((inv*bl+weight*bb)>>6)&31;
  s->colors[i]=red|(green<<5)|(blue<<10);
  a++; b++;
 } while(++i<=15);
}
