#include "global.h"
s32 sub_0822B5D0(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f) { s32 x = a - d; s32 y; if (x < 0) x = -x; y = b - e; if (y < 0) y = -y; c += f; if (x < c && y < c) return 1; return 0; }
