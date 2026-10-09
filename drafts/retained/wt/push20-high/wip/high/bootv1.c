#include "global.h"
struct Dma {vu32 src,dest,control;};
extern const u8 gUnk_0824FFA8[];
extern u8 gUnk_03003A10[],gUnk_03003A60[];
extern u32 gUnk_0300426C,gUnk_03003A44;
void IntrMain(void);
void sub_08213D10(void)
{
 u32 fill,zero;
 struct Dma *dma=(struct Dma*)0x040000d4;
 dma->src=(u32)gUnk_0824FFA8;dma->dest=(u32)gUnk_03003A10;dma->control=0x8400000d;(void)dma->control;
 dma->src=(u32)IntrMain;dma->dest=(u32)gUnk_03003A60;dma->control=0x84000200;(void)dma->control;
 *(u8**)0x03007ffc=gUnk_03003A60;
 zero=0;gUnk_0300426C=zero;gUnk_03003A44=zero;
 fill=zero;dma->src=(u32)&fill;dma->dest=0x06000000;dma->control=0x85006000;(void)dma->control;
 fill=0xa0;dma->src=(u32)&fill;dma->dest=0x07000000;dma->control=0x85000100;(void)dma->control;
 fill=zero;dma->src=(u32)&fill;dma->dest=0x05000000;dma->control=0x85000100;(void)dma->control;
 {u32 mask=0x2001;*(vu16*)0x04000200=mask;}
 *(vu16*)0x04000004=8;*(vu16*)0x04000208=1;
}
