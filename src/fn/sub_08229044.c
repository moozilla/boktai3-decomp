#include "global.h"
void sub_0821AAD8(void *); void sub_0821B4C8(void *, u32, u32); extern u8 gUnk_03005430[];
u8 sub_08229044(void) { u8 r = gUnk_03005430[0xe]; u32 buf[2]; sub_0821AAD8(buf); sub_0821B4C8(buf, 0, gUnk_03005430[0xe]); return r; }
