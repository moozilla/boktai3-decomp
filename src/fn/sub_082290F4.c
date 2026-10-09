#include "global.h"
extern u8 gUnk_03005430[];
s32 sub_08229440(void);s32 sub_08229380(void);void sub_082284CC(s32);
void sub_08228C40(s32,s32,s32,s32,s32,s32,s32);
void sub_082290F4(void)
{
 s32 status=0,i=0,error=-1;
 while(i<=4) {
  status=sub_08229440();
  if(status!=error) break;
  sub_082284CC(200);i++;
 }
 if(status==1) {
  if(sub_08229380()==1) gUnk_03005430[9]=0;
  else {sub_08228C40(2005,7,28,12,0,0,status);gUnk_03005430[9]=status;}
 } else {
  s32 one;
  sub_08228C40(2005,7,28,12,0,0,one=1);gUnk_03005430[9]=one;
 }
}
