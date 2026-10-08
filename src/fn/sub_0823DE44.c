#include "global.h"
u32 sub_0823DE44(u8 *a, u8 *b) { u32 v = *a; u32 r = (v & 0xf) << 8; u32 t = v >> 4; switch (t) { case 1: r -= b[4]; break; case 2: r -= b[0]; break; } return r; }
