#include "global.h"
extern s32 gUnk_03004DA4;
extern u16 gUnk_030051F0[];
extern u8 gUnk_03004DC0[];
u32 sub_08217344(u32 key,void *src) {
 s32 i=0; u32 result;
 for(;i<gUnk_03004DA4;i++) if(gUnk_030051F0[i]==key) goto found;
 if(gUnk_03004DA4>15) goto overflow;
 {
 s32 n=gUnk_03004DA4;
 gUnk_030051F0[n]=key;
 CpuFastSet(src,gUnk_03004DC0+n*32,8);
 return gUnk_03004DA4++;
 }
 found: return i;
 overflow: return 0;
}
