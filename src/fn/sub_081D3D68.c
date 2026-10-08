#include "global.h"
struct S { u8 f[0x1C]; u32 a; };
void sub_081D48D8(void *, u32);
void sub_081D3D68(struct S *p) { sub_081D48D8(p, p->a); }
