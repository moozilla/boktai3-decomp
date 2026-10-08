#include "global.h"
s32 sub_0822B9E4(u32 *p, u32 v) { s32 f = 0x10; s32 i = 0; p += 0x26; do { u32 e = *p; if (e == v) return i; if (e == 0) f = i; p++; i++; } while (i <= 0xf); return -f; }
