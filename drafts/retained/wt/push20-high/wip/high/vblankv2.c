#include "global.h"
struct Dma {vu32 src,dest,control;};
extern u32 gUnk_03003A04,gUnk_03003A08,gUnk_03004260;
extern u16 gUnk_03004BD8;
extern u8 gUnk_03004390[];
void sub_082158B0(void);void sub_08213F80(void);void sub_082192F0(void);void sub_082173A8(void);
void sub_0821537C(void);void sub_08218B34(void);void sub_08215964(void);void sub_08019788(void);void sub_08219134(void);
u32 sub_082141C0(void)
{
 u32 *flags;
 vu16 *disp;
 sub_082158B0();sub_08213F80();sub_082192F0();
 flags=&gUnk_03003A04;
 if(*flags&1) {
  struct Dma *dma=(struct Dma*)0x040000d4;
  dma->src=(u32)gUnk_03004390;dma->dest=0x07000000;dma->control=0x84000100;(void)dma->control;
  *flags&=~1;
 }
 sub_082173A8();sub_0821537C();sub_08218B34();sub_08215964();sub_08019788();
 disp=(vu16*)0x04000000;
 *disp&=0xe0ff;
 if(!gUnk_03003A08) {
  u32 mask=gUnk_03004BD8;
  u16 force=0x1000;
  mask|=force;mask|=*disp;*disp=mask;
 }
 sub_08219134();return gUnk_03004260=0;
}
