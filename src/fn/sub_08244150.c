#include "global.h"
struct A { u8 p[0x18]; void *f10; };
extern struct A *gUnk_03006A50;
void sub_08244150(void *v) { gUnk_03006A50->f10 = v; }
