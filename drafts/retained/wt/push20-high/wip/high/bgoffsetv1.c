#include "global.h"
struct Background {u8 pad[0x20];u16 x,y;u8 tail[0xc];};
extern u16 gUnk_03004C20[],gUnk_03004BEC,gUnk_03004BF0[],gUnk_03004BD0[];
extern struct Background gUnk_03004C30[];
extern u32 gUnk_03004C18[],gUnk_03004BF8,gUnk_03004C00;
void sub_082158B0(void)
{
 u16 *dst=gUnk_03004C20;
 struct Background *src=gUnk_03004C30;
 u32 first=src[0].x;
 u32 mask=0xff;
 dst[0]=mask&first;dst[1]=mask&src[0].y;
 dst[2]=mask&src[1].x;dst[3]=mask&src[1].y;
 if(!gUnk_03004BEC) {
  dst[4]=mask&src[2].x;dst[5]=mask&src[2].y;
  dst[6]=mask&src[3].x;dst[7]=mask&src[3].y;
 } else {
  u16 *a=gUnk_03004BF0,*b=gUnk_03004BD0;
  u32 *c;
  a[0]=b[0];a[1]=b[1];a[2]=b[2];a[3]=b[3];
  c=gUnk_03004C18;c[0]=gUnk_03004BF8;c[1]=gUnk_03004C00;
 }
}
