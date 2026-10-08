#include "global.h"
void sub_0824690C(u8 **ps, u8 **pd, s32 n) { u8 *s = *ps; u8 *d = *pd; n--; if (n != -1) { do { *d = *s; s++; d++; n--; } while (n != -1); } *ps = s; *pd = d; }
