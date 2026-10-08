#include "global.h"

struct T { u32 a; void (*b)(void); void (*c)(void); };
void sub_082141BC(void);

void sub_08222D18(void) {
    struct T *t;
    *(vu16 *)0x04000208 = 0;
    *(vu16 *)0x04000200 &= 0xFF7F;
    t = (struct T *)0x03003A10;
    t->b = sub_082141BC;
    *(vu16 *)0x04000208 = 1;
    *(vu16 *)0x04000128 = 0x2003;
    *(vu16 *)0x04000202 = 0x80;
}
