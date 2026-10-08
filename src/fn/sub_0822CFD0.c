#include "global.h"
s32 Script_SeekToKeyword(u32); s32 Script_GetValue(void); void sub_0822CF94(u32);
u32 sub_0822CFD0(void) { u32 a; s32 i; if (Script_SeekToKeyword(0x69) == 0) return 0; a = Script_GetValue(); i = 0xf; do { sub_0822CF94(a); i--; } while (i >= 0); }
