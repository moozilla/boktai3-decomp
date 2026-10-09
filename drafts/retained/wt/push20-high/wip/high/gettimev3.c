#include "global.h"
extern u8 gUnk_03005460[];
void sub_0822C1C8(void);void sub_0822C1F0(void);void sub_082284CC(s32);void sub_082284DC(void);void sub_082284E8(void);
u32 sub_08248C70(void*);
u32 sub_08229258(void)
{
 u32 status=0;
 s32 i;
 u8 *p;
 vu16 *ime,*ie;
 u32 zero;
 sub_0822C1C8();
 i=0;ime=(vu16*)0x04000208;zero=0;ie=(vu16*)0x04000200;
 while(i<=4) {
  u16 old;
  *ime=zero;old=*ie;*ie=zero;
  sub_082284DC();status=(u8)sub_08248C70(gUnk_03005460);sub_082284E8();
  *ie=old;*ime=1;
  if(status==1) break;
  sub_082284CC(200);i++;
 }
 sub_0822C1F0();
 if(status==1) {
  p=gUnk_03005460;
  if((p[0]&0xf0)>0x9f || (15&p[0])>9) {status=-2;p[0]=0;}
  if((u8)(p[1]-1)>0x11 || (15&p[1])>9) {status=-3;p[1]=1;}
  if((u8)(p[2]-1)>0x30 || (15&p[2])>9) {status=-4;p[2]=1;}
  if(p[3]==7) {status=-5;p[3]=0;}
  if(p[4]>0x23 || (15&p[4])>9) {status=-6;p[4]=0;}
  if(p[5]>0x5f || (15&p[5])>9) {status=-7;p[5]=0;}
  if(p[6]>0x5f || (15&p[6])>9) {status=-8;p[6]=0;}
 }
 return status;
}
