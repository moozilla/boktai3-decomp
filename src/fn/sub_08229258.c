#include "global.h"
struct Bytes {u8 b0,b1,b2,b3,b4,b5,b6,b7;};
extern struct Bytes gUnk_03005460;
void sub_0822C1C8(void);void sub_0822C1F0(void);void sub_082284CC(s32);void sub_082284DC(void);void sub_082284E8(void);
u32 sub_08248C70(void*);
u32 sub_08229258(void)
{
 u32 status=0;
 s32 i;
 
 vu16 *ime,*ie;
 u32 zero;
 sub_0822C1C8();
 i=0;ime=(vu16*)0x04000208;zero=0;ie=(vu16*)0x04000200;
 while(i<=4) {
  u16 old;
  *ime=zero;old=*ie;*ie=zero;
  sub_082284DC();status=(u8)sub_08248C70(&gUnk_03005460);sub_082284E8();
  *ie=old;*ime=1;
  if(status==1) break;
  sub_082284CC(200);i++;
 }
 sub_0822C1F0();
 if(status==1) {
  
  if((gUnk_03005460.b0&0xf0)>0x9f || (15&gUnk_03005460.b0)>9) {status=-2;gUnk_03005460.b0=0;}
  if((u8)(gUnk_03005460.b1-1)>0x11 || (15&gUnk_03005460.b1)>9) {status=-3;gUnk_03005460.b1=1;}
  if((u8)(gUnk_03005460.b2-1)>0x30 || (15&gUnk_03005460.b2)>9) {status=-4;gUnk_03005460.b2=1;}
  if(gUnk_03005460.b3==7) {status=-5;gUnk_03005460.b3=0;}
  if(gUnk_03005460.b4>0x23 || (15&gUnk_03005460.b4)>9) {status=-6;gUnk_03005460.b4=0;}
  if(gUnk_03005460.b5>0x5f || (15&gUnk_03005460.b5)>9) {status=-7;gUnk_03005460.b5=0;}
  if(gUnk_03005460.b6>0x5f || (15&gUnk_03005460.b6)>9) {status=-8;gUnk_03005460.b6=0;}
 }
 return status;
}
