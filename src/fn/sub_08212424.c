#include "global.h"
extern u16 gUnk_03004FC0[];
void sub_082156B8(u32);void sub_0821656C(u32,u32,u32,u32,u32);
u8 *sub_0821A520(u32,u32);
void sub_082161B4(u32,u32,void*,u32,u32,u32,u32*);
void sub_08215A74(u32,u32);
void sub_08212424(u8 *p)
{
 u32 size=16;
 u8 *res;
 sub_082156B8(0);sub_0821656C(0,0,0,0,0);
 sub_082156B8(2);sub_0821656C(2,0,0,0,0);
 sub_082156B8(3);sub_0821656C(3,0,0,0,0);
 res=sub_0821A520(0xc091,0x3536);
 *(u8**)(p+0x18)=res;
 size=15;sub_082161B4(2,0,res,0,0,1,&size);
 sub_08215A74(3,2);
 sub_0821656C(3,0,0,0,0);
 CpuSet(sub_0821A520(0x92b3,0x204)+0x14,gUnk_03004FC0,0x100);
}
