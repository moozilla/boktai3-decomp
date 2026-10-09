#include "global.h"
extern u8 *gUnk_02000710;
extern const s16 gUnk_086149C4[];
u32 sub_082215E4(s32,s32);
s32 Div(s32,s32);
void sub_0818D8E0(u8 *);
#define H(o) (*(u16 *)(p+(o)))
#define S(o) (*(s16 *)(p+(o)))
void sub_0818D754(u8 *p)
{
    u8 *owner=*(u8 **)(p+0x15c);
    u8 *angle;
    u16 *startX,*startY,*startZ,*endX,*endY,*endZ;
    u32 value;
    value=sub_082215E4(*(s16 *)(gUnk_02000710+0x30)-S(0x1c),*(s16 *)(gUnk_02000710+0x34)-S(0x20));
    angle=p+0x11a; *angle=value;
    p[0x1dd]=0;
    *(void (**)(u8 *))(p+0x1d0)=sub_0818D8E0;
    H(0x154)=0; H(0x156)=0; H(0x158)=0;
    value=*(u16 *)(owner+0x5ac)-H(0x1c); H(0x14c)=value;
    value=*(u16 *)(owner+0x5ae)-H(0x1e); H(0x14e)=value;
    value=*(u16 *)(owner+0x5b0)-H(0x20); H(0x150)=value;
    value=H(0x1c); startX=(u16 *)(p+0x13c); *startX=value;
    value=H(0x1e); startY=(u16 *)(p+0x13e); *startY=value;
    value=H(0x20); startZ=(u16 *)(p+0x140); *startZ=value;
    value=H(0x1c)+Div(800*gUnk_086149C4[(*angle+64)&255],4096);
    endX=(u16 *)(p+0x134); *endX=value;
    value=*(u16 *)(gUnk_02000710+0x32)+220;
    endY=(u16 *)(p+0x136); *endY=value;
    value=H(0x20)+Div(800*gUnk_086149C4[*angle],4096);
    endZ=(u16 *)(p+0x138); *endZ=value;
    value=*endX-*startX; H(0x144)=value;
    value=*endY-*startY; H(0x146)=value;
    value=*endZ-*startZ; H(0x148)=value;
    H(0x1d6)=0;
}
