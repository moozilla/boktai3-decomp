#include "global.h"
struct Snapshot { u32 a,b;u16 c,d; };
extern struct Snapshot gUnk_030054D0;
extern u32 gUnk_03004278,gUnk_03003A00,gUnk_03002608,gUnk_0300546C;
extern u16 gUnk_03004270,gUnk_03004274,gUnk_0300427C;
void sub_0822B410(void),sub_08213F80(void),sub_082304D4(void),sub_08230554(void),sub_0822C1C8(void),sub_0822C1F0(void);
void sub_0822EDF8(void) {
 struct Snapshot *snap=&gUnk_030054D0;
 *(volatile u16*)0x04000202=1;
 gUnk_0300427C=1;
 sub_08230554();sub_08213F80();
 *(volatile u16*)0x04000208=0;
 gUnk_03004278=snap->a;
 gUnk_03003A00=snap->b;
 gUnk_03004270=snap->c;
 gUnk_03004274=snap->d;
 gUnk_0300546C=gUnk_03002608;
 *(volatile u16*)0x04000208=1;
 return sub_0822C1F0();
}
