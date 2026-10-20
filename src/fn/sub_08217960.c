#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 80.1%
// Blended bank loops still differ in constant hoisting and register allocation.
// fuzzy: 0%
#define Palette_ApplyBackgroundFade sub_08217960
extern s32 gUnk_03004DA0,gUnk_030051D8,gUnk_03004DA8;
extern u16 *gUnk_030051E0;
extern u16 gUnk_03004FC0[],gUnk_030051C4,gUnk_03005214;
extern s16 gUnk_03004DB0,gUnk_030051D0;
u16 *Palette_GetBackgroundBuffer(void);
void Palette_ApplyBackgroundFade(void) {
 u16 *buffer=Palette_GetBackgroundBuffer();
 s32 product=gUnk_03004DA0*gUnk_030051D8;
 s32 value=product>>6;
 s32 i,j; u16 *src,*dst;
 register s32 level asm("r12"); s32 other;u32 rb,g;
 if(value==64) {gUnk_030051E0=gUnk_03004FC0;return;}
 if(value==0) {
 dst=buffer;
 for(i=0;i<16;i++) {
 if((gUnk_030051C4>>i)&1) {
 for(j=15;j>=0;j--) {*dst++=gUnk_03005214;}
 } else {src=gUnk_03004FC0+i*16;for(j=15;j>=0;j--) {*dst++=*src++;}}
 }
 gUnk_030051E0=buffer;return;
 }
 level=product>>7;other=32-level;
 gUnk_030051E0=buffer;
 if(!gUnk_03005214) {
 src=gUnk_03004FC0;dst=buffer;
 for(i=0;i<16;i++) {
 if((gUnk_030051C4>>i)&1) {
 for(j=15;j>=0;j--) {u32 c=*src;*dst=(((level*(c&0x7c1f))&0xf83e0)|((level*(c&0x3e0))&0x7c00))>>5;src++;dst++;}
 } else {for(j=15;j>=0;j--) {*dst++=*src++;}}
 }
 } else {
 rb=gUnk_03005214&0x7c1f;g=gUnk_03005214&0x3e0;
 src=gUnk_03004FC0;dst=buffer;
 for(i=0;i<16;i++) {
 if((gUnk_030051C4>>i)&1) {
 for(j=15;j>=0;j--) {u32 c=*src;*dst=(((level*(c&0x7c1f)+rb*other)&0xf83e0)|((level*(c&0x3e0)+g*other)&0x7c00))>>5;src++;dst++;}
 } else {for(j=15;j>=0;j--) {*dst++=*src++;}}
 }
 }
 if(gUnk_03004DB0) {
 u32 c=gUnk_03005214;
 u32 rb=c&0x7c1f,g=c&0x3e0;
 s32 old=gUnk_030051D0;
 gUnk_030051D0=(((level*(old&0x7c1f)+rb*other)&0xf83e0)|((level*(old&0x3e0)+g*other)&0x7c00))>>5;
 }
 gUnk_03004DA8=value;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08217960);
#endif
