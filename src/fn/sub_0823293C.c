#include "global.h"
struct N { u8 p[0x7c]; struct N *next; }; struct L { u8 p[0x1c]; struct N *head; }; extern u32 gUnk_02000474; void sub_082323DC(void *);
u32 sub_0823293C(struct L *l) { struct N *p = l->head; while (p) { struct N *n = p->next; sub_082323DC(p); p = n; } return gUnk_02000474 = 0; }
