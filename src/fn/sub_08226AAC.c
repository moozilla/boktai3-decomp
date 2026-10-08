#include "global.h"
struct S { u8 a[0x1e]; u16 h; };
void sub_08226A7C(struct S *); void sub_08226920(struct S *);
void sub_08226AAC(struct S *p) { u32 v = p->h; if (v == 1) sub_08226A7C(p); else if (v == 2) sub_08226920(p); }
