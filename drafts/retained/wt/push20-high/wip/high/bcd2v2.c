#include "global.h"
s32 Mod(s32,s32);
s32 Div(s32,s32);
u32 sub_0822874C(s32 a,s32 b,s32 c)
{
 s32 d0,d1,d2,d3,e0,e1,f0,f1,sum,tmp;
 u32 result;
 d0=Mod(a,10);
 d1=Div(Mod(a,100)-d0,10);
 tmp=Mod(a,1000);
 sum=d0+d1;
 d2=Div(tmp-sum,100);
 tmp=Mod(a,10000);
 sum+=d2;
 d3=Div(tmp-sum,1000);
 e0=Mod(b,10);
 e1=Div(Mod(b,100)-e0,10);
 f0=Mod(c,10);
 f1=Div(Mod(c,100)-f0,10);
 result=(u32)d3<<28;
 result|=(u32)d2<<24;
 result|=(u32)d1<<20;
 result|=(u32)d0<<16;
 result|=(u32)e1<<12;
 result|=(u32)e0<<8;
 result|=(u32)f1<<4;
 result|=f0;
 return result;
}
