#include "global.h"
struct A { u8 p[0x20]; void *f10; };
extern struct A *gUnk_03006A50;
void sub_08244168(void *v) { gUnk_03006A50->f10 = v; }
