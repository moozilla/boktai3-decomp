#include "global.h"
extern u16 gUnk_03004BD8;
extern u32 gUnk_03004BC0[];
extern u16 gUnk_03004BE0[],gUnk_03004C08[];
extern const u32 gUnk_086140CC[];
void sub_082156B8(s32);
struct Dma {vu32 src,dst,ctl;};
void sub_082157EC(void)
{
 u16 initial;
 u32 zero;
 vu16 *bg,*blend;
 struct Dma *dma;
 u16 *a,*b;
 u32 *p;
 const u32 *src;
 u32 dst;
 s32 i;
 gUnk_03004BD8=0;initial=0;
 sub_082156B8(0);sub_082156B8(1);sub_082156B8(2);sub_082156B8(3);
 bg=(vu16*)0x04000008;
 *bg&=0xffbf;bg++;*bg&=0xffbf;bg++;*bg&=0xffbf;
 {vu16 *last=(vu16*)0x0400000e;*last&=0xffbf;}
 blend=(vu16*)0x04000050;*blend=initial;blend++;*blend=initial;blend++;*blend=initial;
 i=0;zero=0;dma=(struct Dma*)0x040000d4;a=gUnk_03004BE0;
 p=gUnk_03004BC0;src=gUnk_086140CC;dst=0x06000000;b=gUnk_03004C08;
 do {
  *p++=zero;*a=zero;*b=zero;
  dma->src=(u32)src;dma->dst=dst;dma->ctl=0x84000008;dma->ctl;
  a++;dst+=0x4000;b++;i++;
 } while(dst<=0x0600c000);
}
