#include "global.h"

u32 sub_08228D7C(void);
void sub_082286E0(u32 *, u32 *, u32 *, u32);

u32 sub_081EA7CC(void) {
    u32 a;
    u32 b;
    u32 c;
    sub_082286E0(&a, &b, &c, sub_08228D7C());
    return b;
}
