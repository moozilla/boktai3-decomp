#include "global.h"
extern u8 gUnk_03005430[];
extern u32 gUnk_03005404;
void sub_082286E0(s32*,s32*,s32*,u32);
u32 sub_0822874C(s32,s32,s32);s32 sub_08228548(s32,s32);s32 sub_0822867C(s32,s32,s32);
void sub_08228848(void)
{
 if(++gUnk_03005430[7]>59) {
  u8 *p;
  gUnk_03005430[7]=0;
  p=gUnk_03005430;
  if(++p[6]>59) {
   p[6]=0;
   if(++p[5]>59) {
    p[5]=0;
    if(++p[4]>23) {
     s32 year,month,day;
     p[4]=0;
     sub_082286E0(&year,&month,&day,*(u32*)p);
     day++;
     if(day>=sub_08228548(year,month)) {
      day=1;month++;
      if(month>12) {
       month=1;year++;
       if(year>2098) year=2099;
      }
     }
     *(u32*)p=sub_0822874C(year,month,day);
     p[8]=sub_0822867C(year,month,day);
    }
   }
   gUnk_03005404=1;
   {u8 *other=gUnk_03005430;
   if(*(u16*)(other+12)==*(u16*)(p+4)) {
    u16 *counter=(u16*)(other+20);
    if(++*counter>280) *counter=0;
   }}
  }
 }
}
