#include "global.h"
u32 Script_SeekToKeyword(u32); u32 Script_GetValue(void); u32 Script_GetPc(void); void sub_0822B640(void);
extern u8 *gUnk_030025FC;
u32 sub_0822B8EC(u8 *p) { if (Script_SeekToKeyword(0x61)) { u32 *q = (u32 *)(p + 0x98); while (Script_GetPc()) *q++ = Script_GetValue(); } sub_0822B640(); gUnk_030025FC = p; return 1; }
