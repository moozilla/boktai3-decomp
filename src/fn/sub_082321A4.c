#include "global.h"
struct S { u8 p[0x20]; u8 *head; }; void sub_08249240(void *, void *, u32);
u32 sub_082321A4(struct S *s) { u8 *q = s->head; while (q) { sub_08249240(s, q, *(u32 *)(q + 0x12c)); q = *(u8 **)(q + 0x134); } return 0; }
