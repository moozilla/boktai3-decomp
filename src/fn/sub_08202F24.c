#include "global.h"
struct O { u8 p[0x40]; u32 f4c; u32 f50; };
void sub_08202F24(struct O *o, u32 a, u32 b) { o->f4c = a; o->f50 = b; }
