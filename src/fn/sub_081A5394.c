#include "global.h"
extern u16 gUnk_03005260;
static inline u32 tst(volatile u16 *a,u32 m) { return *a&m; }
void sub_081A5394(u16 *p) {
 if(gUnk_03005260&1) {
 if(gUnk_03005260&0x10) p[0]+=2;
 else if(gUnk_03005260&0x20) p[0]-=2;
 if(gUnk_03005260&0x40) p[1]-=2;
 else if(gUnk_03005260&0x80) p[1]+=2;
 }
}
