#include "global.h"
s32 Div(s32,s32);s32 Mod(s32,s32);
s32 sub_0822867C(s32 year,s32 month,s32 day)
{
 s32 a,b,c,d,sum;
 if(month<=2) { year--;month+=12; }
 a=Div(year,4);b=Div(year,100);c=Div(year,400);
 d=Div(13*month+8,5);
 sum=year+a;sum-=b;sum+=c;sum+=d;sum+=day;
 return Mod(sum,7);
}
