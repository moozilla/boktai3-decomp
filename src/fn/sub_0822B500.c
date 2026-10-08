#include "global.h"
extern u16 gUnk_03005470[]; void m4aSongNumStop(u16);
void sub_0822B500(void) { u16 *p = gUnk_03005470; if (p[0x3a/2] != 0) m4aSongNumStop(p[0x3a/2]); if (p[0x3c/2] != 0) m4aSongNumStop(p[0x3c/2]); }
