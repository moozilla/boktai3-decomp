#include "global.h"
struct P { u8 p[8]; u16 *f8; };
u32 sub_0821E7DC(struct P *p) { return p->f8[2] >> 8; }
