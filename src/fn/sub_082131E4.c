#include "global.h"
s32 sub_08213010(void*,s32);
s32 Mod(s32,s32);s32 Div(s32,s32);
void sub_0821980C(void*,void*,u32,u32);
u32 sub_082131E4(u8 *p)
{
 s32 value=sub_08213010(p,*(s16*)(p+0x722));
 s32 page=*(s16*)(p+0x6f4);
 if(value>=page*10 && value<(page+1)*10) {
  s32 digit=Mod(value,10),a,b,x,y;
  a=Mod(digit,5);b=Div(digit,5);
  x=a*40+38;
  *(u16*)(p+0x384)=x;
  y=b*40+38;
  *(u16*)(p+0x386)=y;
  *(u32*)(p+0x36c)&=~1;
  sub_0821980C(p+0x364,p+0x44,0x50,0);
 } else {
  *(u32*)(p+0x36c)|=1;
 }
 return 0;
}
