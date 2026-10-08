#include "global.h"
u32 Script_SeekToKeyword(u32); u32 Script_GetPc(void); void Script_ParseStringRef(void); u8 *Text_LookupString(void);
extern u16 gUnk_03005428;
void sub_0822824C(void) { if (Script_SeekToKeyword(0x73)) { if (Script_GetPc()) { u8 *t; Script_ParseStringRef(); t = Text_LookupString(); gUnk_03005428 = (t[1] | (t[0] << 8)) & 0xFFF; } } }
