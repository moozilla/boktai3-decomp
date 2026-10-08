#include "global.h"
s32 Script_SeekToKeyword(u32); s32 Script_GetValue(void); u32 sub_0822CEE8(u32, u32);
u32 sub_0822CF0C(void) { u32 a, b; if (Script_SeekToKeyword(0x69) == 0) return 0; a = Script_GetValue(); if (Script_SeekToKeyword(0x70)) b = Script_GetValue(); else b = 0; return sub_0822CEE8(a, b); }
