#include "global.h"
s32 Script_SeekToKeyword(u32); s32 Script_GetValue(void); u32 sub_0822D490(u32, u32);
u32 sub_0822D5D0(void) { u32 a, r; if (Script_SeekToKeyword(0x6b) == 0) r = 0; else { a = Script_GetValue(); if (Script_SeekToKeyword(0x69) == 0) r = 0; else r = sub_0822D490(a, Script_GetValue()); } return r; }
