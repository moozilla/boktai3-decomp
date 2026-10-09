#include "global.h"
void sub_081DF1D8(void *);void sub_0822B2F8(s32);
void sub_081DF694(u8 *p) {
 switch(*(u32 *)(p+0x34c)) {
case 0:*(u32 *)(p+0x34c)=0x3d;break;
case 5:*(u32 *)(p+0x34c)=0x3f;break;
case 10:*(u32 *)(p+0x34c)=0x40;break;
case 15:*(u32 *)(p+0x34c)=0x41;break;
case 20:*(u32 *)(p+0x34c)=0x41;break;
case 25:*(u32 *)(p+0x34c)=0x41;break;
case 30:*(u32 *)(p+0x34c)=4;break;
case 35:*(u32 *)(p+0x34c)=9;break;
case 40:*(u32 *)(p+0x34c)=0xe;break;
case 45:*(u32 *)(p+0x34c)=0x13;break;
case 50:*(u32 *)(p+0x34c)=0x18;break;
case 55:*(u32 *)(p+0x34c)=0x1d;break;
case 60:*(u32 *)(p+0x34c)=0x22;break;
case 62:*(u32 *)(p+0x34c)=0x27;break;
case 64:*(u32 *)(p+0x34c)=0x2c;break;
case 65:*(u32 *)(p+0x34c)=0x31;break;
default:*(u32 *)(p+0x34c)-=1;break;
 }
 sub_081DF1D8(p);sub_0822B2F8(0xdc);
}
