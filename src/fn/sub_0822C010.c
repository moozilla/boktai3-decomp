#include "global.h"
extern u8 *gUnk_02000710; extern u32 gUnk_030054B0;
s32 Script_GetValue(void);
void sub_0822C010(void) { *(s32 *)(gUnk_02000710 + 0x5a0) = Script_GetValue(); gUnk_030054B0 = 1; }
