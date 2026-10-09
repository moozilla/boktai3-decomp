#include "global.h"
void sub_082286E0(s32 *a, s32 *b, s32 *c, s32 value)
{
 s32 d0,d1,d2,d3,d4,d5,d6,d7,total,total2,total3;
 d0=(u32)value>>28;
 total=d0*1000;
 d1=value & 0x0f000000;
 d1>>=24;
 d1*=100;
 total+=d1;
 d2=value & 0x00f00000;
 d2>>=20;
 total+=d2*10;
 d3=value & 0x000f0000;
 d3>>=16;
 total+=d3;
 *a=total;
 d4=value & 0x0000f000;
 d4>>=12;
 total2=d4*10;
 d5=value & 0x00000f00;
 d5>>=8;
 total2+=d5;
 *b=total2;
 d6=value & 0x000000f0;
 d6>>=4;
 total3=d6*10;
 d7=value&15;
 total3+=d7;
 *c=total3;
}
