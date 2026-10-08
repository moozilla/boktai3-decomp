#include "global.h"
void sub_081DAD0C(void *, void (*)(void));
void sub_081D8FAC(void);
struct B { u8 pad[0x10]; u16 h; };
u32 sub_081D8F8C(u8 *p) {
    sub_081DAD0C(p + 0x18, sub_081D8FAC);
    p += 0x8c;
    ((struct B *)p)->h = 0x19;
    return 0;
}
