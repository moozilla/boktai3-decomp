#include "global.h"
u32 Script_SeekToKeyword(u32); u32 Script_GetValue(void); void sub_0822AF10(u32);
void sub_0822AF5C(void) { u32 v; if (Script_SeekToKeyword(0x69)) v = Script_GetValue(); else v = 0; sub_0822AF10(v); }
