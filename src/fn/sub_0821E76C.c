#include "global.h"
struct A { u8 p[0x10]; u8 *f10; };
extern struct A *gUnk_030052F4;
u8 *sub_0821E76C(u16 *d) { u8 *b = gUnk_030052F4->f10; return b + d[1]; }
