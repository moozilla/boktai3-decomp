#include "global.h"
extern void *gUnk_02000708;extern u32 *gUnk_02000710;extern u32 gUnk_0200060C,gUnk_03005404;
u32 sub_0822ED64(u32);u32 sub_0822BAD0(u32,u32),sub_0822BB38(void);
s32 sub_0822F10C(u32,void*,u32);
u32 sub_0822BC6C(u32 arg) {
 u32 size=sub_0822ED64(1024);
 if(sub_0822F10C((u16)sub_0822BAD0(arg,0),gUnk_02000708,size)<0) return 0;
 size=sub_0822ED64(sub_0822BB38());
 if(sub_0822F10C((u16)sub_0822BAD0(arg,1),gUnk_02000710,size)<0) return 0;
 if(*gUnk_02000710!=gUnk_0200060C) return 0;
 return gUnk_03005404=1;
}
