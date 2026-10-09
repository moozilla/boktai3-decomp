#include "global.h"
u16 sub_08228508(u16 value)
{
 u32 word=(u32)value<<16;
 u32 mask=15;
 u32 result=(word&0x000f0000)>>16;
 u32 a=(word>>20)&mask;
 u32 b,c;
 result+=10*a;
 b=(word>>24)&mask;result+=100*b;
 c=(word>>28)&mask;result+=1000*c;
 return result;
}
