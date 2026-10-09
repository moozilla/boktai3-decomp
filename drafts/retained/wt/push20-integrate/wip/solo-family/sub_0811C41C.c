#include "global.h"
struct State { u8 pad0[0x14]; s32 value,limit; };
s32 Div(s32,s32);
void sub_0811C41C(u8 *p,struct State *s)
{
    s32 six=Div(s->limit*6,10);
    s32 four=Div(s->limit*4,10);
    s32 two=Div(s->limit*2,10);
    u32 result;
    if(s->value<=two) { p[0]=20; p[2]=2; p[3]=1; return; }
    else if(s->value<=four) { p[0]=40; p[2]=2; p[3]=1; return; }
    else if(s->value<=six) { p[0]=60; p[2]=1; p[3]=1; return; }
    else p[3]=0;
}
