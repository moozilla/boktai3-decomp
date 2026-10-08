#include "global.h"
struct S { u8 p[8]; u8 b; };
extern struct S *gUnk_030053F8;
u8 sub_0822BB7C(void);
u8 sub_0822BEC0(void) { struct S *s = gUnk_030053F8; s->b = 1 - s->b; return sub_0822BB7C(); }
