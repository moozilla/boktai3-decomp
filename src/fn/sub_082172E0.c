#include "global.h"
extern s32 gUnk_030051CC;
extern u16 gUnk_030051F0[];
extern u8 gUnk_03004DC0[];
u32 sub_082172E0(u32 key,void *src) {
 s32 i=0; u32 result;
 for(;i<gUnk_030051CC;i++) if(gUnk_030051F0[i]==key) goto found;
 if(gUnk_030051CC>1) goto overflow;
 {
 s32 n=gUnk_030051CC;
 gUnk_030051F0[n]=key;
 CpuFastSet(src,gUnk_03004DC0+n*32,8);
 return gUnk_030051CC++;
 }
 found: return i;
 overflow: return 0;
}
