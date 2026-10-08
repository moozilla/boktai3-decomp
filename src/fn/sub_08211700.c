#include "global.h"
s32 sub_082116EC(s32);
s32 sub_08211700(s32 n) { s32 s = 0; s32 i; if (n == 0) return 0; for (i = 0; i < n; i++) s += sub_082116EC(i); return s; }
