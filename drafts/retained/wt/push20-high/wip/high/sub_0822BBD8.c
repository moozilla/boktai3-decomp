#include "global.h"
extern u8 *gUnk_02000710;extern u32 gUnk_0200060C,gUnk_03005404;
void sub_0821B1B8(void*,u32);
u8 *sub_0822ED64(u32);u32 sub_0822BAD0(u32,u32);
void *sub_0821B6BC(void),*sub_0821B6B0(void);
u8 *sub_0822F0A4(u32,void*,u8*);u32 sub_0822BB38(void);
u32 sub_0822BBD8(u32 arg) {
 u8 *buffer,*result;u32 index;
 *(u32*)gUnk_02000710=gUnk_0200060C;
 sub_0821B1B8(gUnk_02000710,4);
 buffer=sub_0822ED64(1024);
 index=sub_0822BAD0(arg,0);index=(u16)index;
 result=sub_0822F0A4(index,sub_0821B6BC(),buffer);
 if(result!=buffer+8) return 0;
 buffer=sub_0822ED64(sub_0822BB38());
 index=sub_0822BAD0(arg,1);index=(u16)index;
 result=sub_0822F0A4(index,sub_0821B6B0(),buffer);
 if(result!=buffer+8) return 0;
 return gUnk_03005404=1;
}
