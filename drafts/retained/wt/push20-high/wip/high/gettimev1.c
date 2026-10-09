#include "global.h"
extern u8 gUnk_03005460[];
void sub_0822C1C8(void);void sub_0822C1F0(void);void sub_082284CC(s32);void sub_082284DC(void);void sub_082284E8(void);
u32 sub_08248C70(void*);
s32 sub_08229258(void)
{
 s32 status=0,i;
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
  u8 *p=gUnk_03005460;
  u8 v=p[0];
  if((v&0xf0)>0x9f || (v&15)>9) {status=-2;p[0]=0;}
  v=p[1];if((u8)(v-1)>0x11 || (v&15)>9) {status=-3;p[1]=1;}
  v=p[2];if((u8)(v-1)>0x30 || (v&15)>9) {status=-4;p[2]=1;}
  if(p[3]==7) {status=-5;p[3]=0;}
  v=p[4];if(v>0x23 || (v&15)>9) {status=-6;p[4]=0;}
  v=p[5];if(v>0x5f || (v&15)>9) {status=-7;p[5]=0;}
  v=p[6];if(v>0x5f || (v&15)>9) {status=-8;p[6]=0;}
 }
 return status;
}
