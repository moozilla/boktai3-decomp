#include "global.h"
void sub_08020D68(void *, u32);
struct S { u8 f0[5]; u8 b5; u8 f1; u8 b7; u8 b8; u8 f2[0xb]; u32 c14; };
void sub_081CEC24(u32 a, struct S *p) {
    if (p->b8) {
        p->b8 = 0;
        p->b7 = 0;
        p->b5 = 0x19;
    }
    if (p->b7) sub_08020D68((u8 *)p + 0x168, 1);
    p->c14++;
}
