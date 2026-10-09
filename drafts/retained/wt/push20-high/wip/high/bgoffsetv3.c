#include "global.h"
struct Background {u8 pad[0x20];u16 x0,y0;u8 p24[0x2c];u16 x1,y1;u8 p54[0x2c];u16 x2,y2;u8 p84[0x2c];u16 x3,y3;};
extern u16 gUnk_03004C20[],gUnk_03004BEC,gUnk_03004BF0[],gUnk_03004BD0[];
extern struct Background gUnk_03004C30;
extern u32 gUnk_03004C18[],gUnk_03004BF8,gUnk_03004C00;
void sub_082158B0(void)
{
 u16 *dst=gUnk_03004C20;
 struct Background *src=&gUnk_03004C30;
 u16 first=src->x0;
 u32 mask=0xff;
 dst[0]=mask&first;dst[1]=mask&src->y0;
 dst[2]=mask&src->x1;dst[3]=mask&src->y1;
 if(!gUnk_03004BEC) {
  dst[4]=mask&src->x2;dst[5]=mask&src->y2;
  dst[6]=mask&src->x3;dst[7]=mask&src->y3;
 } else {
  u16 *a=gUnk_03004BF0,*b=gUnk_03004BD0;
  u32 *c;
  a[0]=b[0];a[1]=b[1];a[2]=b[2];a[3]=b[3];
  c=gUnk_03004C18;c[0]=gUnk_03004BF8;c[1]=gUnk_03004C00;
 }
}
