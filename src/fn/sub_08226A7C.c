#include "global.h"
struct S { u8 a[0x96]; u16 h96; u32 w98; };
void sub_0821AFB4(u32, u32); void sub_0821AD08(u32, u32);
void sub_08226A7C(struct S *p) { u32 w = p->w98; if (w != 0) { if (p->h96 == 0) { p->w98 = 0; sub_0821AFB4(w, 0); } else { p->w98 = 0; sub_0821AD08(w, 0); } } }
