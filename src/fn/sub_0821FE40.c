#include "global.h"
struct S { u8 a[6]; u16 h6; };
void sub_0821FE04(struct S *); void sub_0821FDC8(struct S *);
void sub_0821FE40(struct S *p) { if (p->h6 & 0x2000) { if ((p->h6 & 4) == 0) sub_0821FE04(p); } else sub_0821FDC8(p); }
