#include "global.h"
void sub_081DF1D8(void *);void sub_0822B2F8(s32);
void sub_081DF32C(u8 *p) {
 switch(*(u32 *)(p+0x34c)) {
case 0:*(u32 *)(p+0x34c)=25;break;
case 1:*(u32 *)(p+0x34c)=26;break;
case 2:*(u32 *)(p+0x34c)=27;break;
case 3:*(u32 *)(p+0x34c)=28;break;
case 4:*(u32 *)(p+0x34c)=29;break;
case 30:*(u32 *)(p+0x34c)=55;break;
case 31:*(u32 *)(p+0x34c)=56;break;
case 32:*(u32 *)(p+0x34c)=57;break;
case 33:*(u32 *)(p+0x34c)=58;break;
case 34:*(u32 *)(p+0x34c)=59;break;
case 60:*(u32 *)(p+0x34c)=65;break;
case 61:*(u32 *)(p+0x34c)=65;break;
case 62:*(u32 *)(p+0x34c)=60;break;
case 63:*(u32 *)(p+0x34c)=61;break;
case 64:*(u32 *)(p+0x34c)=62;break;
case 65:*(u32 *)(p+0x34c)=64;break;
default:*(u32 *)(p+0x34c)-=5;break;
 }
 sub_081DF1D8(p);sub_0822B2F8(0xdc);
}
