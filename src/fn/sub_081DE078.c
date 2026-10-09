#include "global.h"
extern u16 gUnk_03004BD8;
void sub_082195E0(void *);
void sub_081DE078(u8 *p) {
 u16 *q=&gUnk_03004BD8;u32 m=~3;
 *q=m&*q;
 sub_082195E0(p+0x2c0);sub_082195E0(p+0x238);sub_082195E0(p+0x1b0);
 sub_082195E0(p+0x128);sub_082195E0(p+0xa0);sub_082195E0(p+0x18);
}
