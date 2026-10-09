#include "global.h"
extern u8 gUnk_03005460[];
s32 sub_08229258(void);
u16 sub_08228508(u8);
void sub_08228C40(u32,u32,u32,u32,u32,u32,u32);
s32 sub_08229380(void)
{
 s32 result=sub_08229258();
 if(result == 1) {
  u32 year=(u16)sub_08228508(gUnk_03005460[0]);
  u16 month,day,hour,minute,second;
  year+=2000;
  month=sub_08228508(gUnk_03005460[1]);
  day=sub_08228508(gUnk_03005460[2]);
  hour=sub_08228508(gUnk_03005460[4]);
  minute=sub_08228508(gUnk_03005460[5]);
  second=sub_08228508(gUnk_03005460[6]);
  sub_08228C40(year,month,day,hour,minute,second,0);
 }
 return result;
}
