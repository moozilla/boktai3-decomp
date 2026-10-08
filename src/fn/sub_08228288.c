#include "global.h"
u32 Script_GetValue(void); extern u8 *gUnk_02000710;
void sub_08228288(void) { u32 v = Script_GetValue(); *(u32 *)(gUnk_02000710 + 0x5a4) = v; }
