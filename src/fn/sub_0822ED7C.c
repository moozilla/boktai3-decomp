#include "global.h"
struct Snapshot { u32 a,b;u16 c,d; };
extern struct Snapshot gUnk_030054D0;
extern u32 gUnk_03004278,gUnk_03003A00,gUnk_03002608,gUnk_0300546C;
extern u16 gUnk_03004270,gUnk_03004274,gUnk_0300427C;
void sub_0822B410(void),sub_08213F80(void),sub_082304D4(void),sub_08230554(void),sub_0822C1C8(void),sub_0822C1F0(void);
u32 sub_0822ED7C(void) {
 gUnk_030054D0.a=gUnk_03004278;
 gUnk_030054D0.b=gUnk_03003A00;
 gUnk_030054D0.c=gUnk_03004270;
 gUnk_030054D0.d=gUnk_03004274;
 gUnk_03004278=0;gUnk_03003A00=0;gUnk_03004270=0;gUnk_03004274=0;
 sub_0822B410();sub_08213F80();
 gUnk_0300427C=0;
 sub_082304D4();sub_08213F80();
 *(volatile u16*)0x04000208=0;
 gUnk_03002608=gUnk_0300546C;
 gUnk_0300546C=1;
 *(volatile u16*)0x04000208=1;
 sub_0822C1C8();return 0;
}
