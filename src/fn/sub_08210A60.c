#include "global.h"
u32 Script_SeekToKeyword(u32); u32 Script_GetValue(void);
extern u8 *gUnk_02000710;
void sub_08210A60(void) { if (Script_SeekToKeyword(0x69)) { u32 v = Script_GetValue(); u32 *q = (u32 *)(gUnk_02000710 + 0x728); *q = *q | (1 << v); } }
