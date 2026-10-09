#include "global.h"
extern u8 gUnk_03005460[];
u8 sub_08228818(u8);
u8 sub_0822867C(s32,s32,s32);
void sub_0822C1C8(void);
void sub_0822C1F0(void);
void sub_082284CC(s32);
void sub_082284DC(void);
void sub_082284E8(void);
u8 sub_08248D20(void*);
u32 sub_08229180(s32 a,s32 b,s32 c,s32 d,s32 e,s32 f)
{
 int i;
 u32 result;
 s32 *last=&f;
 { u8 v=sub_08228818((u8)(a+0x30));
 i=0;
 gUnk_03005460[0]=v; }
 gUnk_03005460[1]=sub_08228818((u8)b);
 gUnk_03005460[2]=sub_08228818((u8)c);
 gUnk_03005460[3]=sub_0822867C(a,b,c);
 gUnk_03005460[4]=sub_08228818((u8)d);
 gUnk_03005460[5]=sub_08228818((u8)e);
 gUnk_03005460[6]=sub_08228818((u8)*last);
 gUnk_03005460[7]=i;
 result=0;
 sub_0822C1C8();
 while(i<=4) {
  u16 old;
  *(vu16*)0x04000208=0;
  old=*(vu16*)0x04000200;
  *(vu16*)0x04000200=0;
  sub_082284DC();
  result=(u8)sub_08248D20(gUnk_03005460);
  sub_082284E8();
  *(vu16*)0x04000200=old;
  *(vu16*)0x04000208=1;
  if(result==1) break;
  sub_082284CC(200);
  i++;
 }
 sub_0822C1F0();
 return result;
}
