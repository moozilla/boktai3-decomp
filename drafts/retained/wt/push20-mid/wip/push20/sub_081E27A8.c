#include "global.h"
extern u8 gUnk_03004FC0[];
void sub_082156B8(u32);
void sub_0821656C(u32,u32,u32,u32,u32);
void *sub_0821A520(u32,u32);
void sub_082161B4(u32,u32,void *,u32,u32,u32,u32 *);
void sub_081DC1B4(u32);
s32 sub_081E27A8(u8 *s)
{
    u32 zero,value;
    void *p,*q;
    *(u32 *)(s+0x24)=4;
    *(u32 *)(s+0x28)=4;
    *(u32 *)(s+0x2C)=300;
    if(Script_SeekToKeyword(0x66)) {
        *(u32 *)(s+0x24)=Script_GetValue();
        *(u32 *)(s+0x28)=Script_GetValue();
    }
    if(Script_SeekToKeyword(0x64)) *(u32 *)(s+0x2C)=Script_GetValue();
    if(Script_SeekToKeyword(0x74)) *(u32 *)(s+0x30)=Script_GetValue();
    if(Script_SeekToKeyword(0x65)) *(u32 *)(s+0x34)=Script_GetValue();
    sub_082156B8(0);
    sub_0821656C(0,0,0,0,zero=0);
    sub_082156B8(2);
    sub_0821656C(2,0,0,0,zero);
    sub_082156B8(3);
    p=sub_0821A520(0xC091,0x52AE);
    value=*(u32 *)(s+0x30);
    sub_082161B4(2,0,p,0,zero,1,&value);
    sub_0821656C(3,0,0,0,zero);
    q=(u8 *)sub_0821A520(0x92B3,0xD985)+0x14;
    CpuSet(q,gUnk_03004FC0,0x100);
    sub_081DC1B4(*(u32 *)(s+0x24));
    return 0;
}
