#include "global.h"
extern u8 *gUnk_03002600; s32 Script_SeekToKeyword(u32); s32 Script_GetValue(void); u8 *Script_GetPc(void);
u32 sub_0822C098(u32 *p) { if (Script_SeekToKeyword(0x6d)) { u32 *q = p + 6; while (Script_GetPc()) *q++ = Script_GetValue(); } gUnk_03002600 = (u8 *)p; return 0; }
