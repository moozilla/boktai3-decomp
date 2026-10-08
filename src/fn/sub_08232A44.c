#include "global.h"
struct S { u8 p[2]; u8 b; u8 pad; u32 c; u32 d; };
static inline u8 t(struct S *s) { if (s->b != 0) { s->b = 0; return 1; } return 0; }
void sub_08232A44(u32 a, struct S *s) { if (t(s)) s->d |= 1; s->c++; }
