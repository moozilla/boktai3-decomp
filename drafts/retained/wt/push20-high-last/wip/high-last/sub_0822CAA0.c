#include "global.h"
extern u8 *gUnk_02000710;extern u8 gUnk_03005430[];
u32 sub_08228D7C(void);s32 sub_08228D88(void),sub_08228D94(void);
void sub_0822CAA0(void) {
 s32 minute,hour;u32 flag=0;
 *(u32*)(gUnk_02000710+0x4e0)=sub_08228D7C();
 *(s32*)(gUnk_02000710+0x4e4)=sub_08228D88();
 minute=sub_08228D94();
 *(s32*)(gUnk_02000710+0x4e8)=minute;
 hour=*(s32*)(gUnk_02000710+0x4e4);
 if(hour<gUnk_03005430[12] || (hour==gUnk_03005430[12] && minute<gUnk_03005430[13])) flag=1;
 *(u32*)(gUnk_02000710+0x4ec)=flag;
}
