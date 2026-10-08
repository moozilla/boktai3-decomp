#include "global.h"
u32 sub_0821F840(u16 *p) { u32 r; if ((*p & 2) == 0) r = 0; else { *p &= 0xFFFD; r = 1; } return r; }
