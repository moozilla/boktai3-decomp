#include "global.h"
extern u8 *gUnk_030042E4;
struct S48E {u8 pad0[0x24];u16 colors[16];u8 pad1[0x37c-0x44];u16 index;u8 pad2[0x394-0x37e];u16 blend;};
void sub_08048E0C(struct S48E *s)
{
 u16 *a=(u16 *)(gUnk_030042E4+(s->index+0x283)*32);
 u16 *b=(u16 *)(gUnk_030042E4+0x5160);
 s32 inv=64-s->blend;
 s32 weight=s->blend;
 s32 i=0;
 do {
  u16 c=*a;
  s32 r=31&c;
  s32 g=(((u32)c<<16)>>21)&31;
  s32 bl=(((u32)c<<16)>>26)&31;
  u16 d=*b;
  s32 rr=31&d;
  s32 gg=(((u32)d<<16)>>21)&31;
  s32 bb=(((u32)d<<16)>>26)&31;
  s32 red=((inv*r+weight*rr)>>6)&31;
  s32 green=((inv*g+weight*gg)>>6)&31;
  s32 blue=((inv*bl+weight*bb)>>6)&31;
  s->colors[i]=red|(green<<5)|(blue<<10);
  a++; b++;
 } while(++i<=15);
}
