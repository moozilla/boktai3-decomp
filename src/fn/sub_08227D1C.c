#include "global.h"
u32 Script_GetValue(void);
extern u16 gUnk_03004BD8;
void sub_08227D1C(void) { u16 n = Script_GetValue(); u32 m = (0x1000000u << n) >> 16; gUnk_03004BD8 = gUnk_03004BD8 & ~m; }
