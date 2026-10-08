#include "global.h"
struct G { u8 a[0x10]; u32 w10; };
extern struct G *gUnk_030053F8;
u32 sub_082281CC(void) { if (gUnk_030053F8->w10 == 0x369F) return 1; return 0; }
