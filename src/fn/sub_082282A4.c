#include "global.h"
extern const u8 gUnk_08E87968[]; extern const u8 gUnk_08E8796E[];
u8 sub_082282A4(s32 a) { s32 n = gUnk_08E87968[0]; s32 i = 0; do { if (a > n) { n += gUnk_08E87968[i]; i++; continue; } return gUnk_08E8796E[i]; } while (i <= 5); return gUnk_08E8796E[0]; }
