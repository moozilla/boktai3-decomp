#include "global.h"
extern u32 gUnk_03004294,gUnk_030042B8,gUnk_030042B4,gUnk_03003A50;
extern s32 gUnk_030051E4;
void sub_082172C8(void);void sub_082152AC(void);void sub_08249238(u32);void sub_082154E4(void);
void sub_08217C7C(void);void sub_08217BBC(void);void sub_08217D50(void);void sub_08217960(void);void sub_08217430(void);
void sub_08215490(u32);
u32 sub_0821425C(void)
{
 sub_082172C8();sub_082152AC();
 sub_08249238(gUnk_03004294);sub_08249238(gUnk_030042B8);sub_08249238(gUnk_030042B4);
 sub_082154E4();
 if(gUnk_030051E4) {
  sub_08217C7C();sub_08217BBC();
  if(gUnk_030051E4>0) gUnk_030051E4--;
  if(!gUnk_030051E4) sub_08217D50();
 } else {sub_08217960();sub_08217430();}
 sub_08215490(0);gUnk_03003A50++;
 return 0;
}
