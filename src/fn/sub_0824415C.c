#include "global.h"
struct A { u8 p[0x1C]; void *f10; };
extern struct A *gUnk_03006A50;
void sub_0824415C(void *v) { gUnk_03006A50->f10 = v; }
