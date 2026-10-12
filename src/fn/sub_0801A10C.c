#include "global.h"
#define SelectionMenu_ConfigureLayers sub_0801A10C
u8 *sub_0821A520(int,int);
void sub_082161B4(int,int,void *,int,int,int,void *);
void sub_0821656C(int,int,int,int,int);
extern u8 gUnk_03005160[];
int SelectionMenu_ConfigureLayers(int value,int mode) {
 u8 *resource=sub_0821A520(0xc091,0xa597);int result;
 if(mode) {
  sub_082161B4(3,0,resource,0,0,1,&value);
  sub_0821656C(1,0,0,0,0);result=1;
 } else {
  sub_082161B4(0,0,resource,0,0,1,&value);
  sub_0821656C(0,0,0,0,0);result=0;
 }
 {u8 *palette=sub_0821A520(0x92b3,0x81b);
 CpuSet(palette+0x1b4,gUnk_03005160,0x30);}
 return result;
}
