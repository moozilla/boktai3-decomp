#include "global.h"
// MATCHING_CONCESSION: register-pinning
extern u16 gUnk_03004BD8;
void sub_082195E0(void*);
// Release all five presentation layers and restore display-control bits.
s32 sub_081E2D40(u8*p){u8*layer; s32 i;
 {s32 value=gUnk_03004BD8;register s32 mask asm("r1")=-3;mask&=value;{register s32 mask2 asm("r2")=-4;mask&=mask2;gUnk_03004BD8=mask;}}
 layer=p+0x38;
 for(i=4;i>=0;i--){sub_082195E0(layer);layer+=0x60;}
 return 0;
}
