#include "global.h"

struct T { u32 a; void (*b)(void); void (*c)(void); };
void sub_082141BC(void);

void sub_08222784(void) {
    u16 ie;
    struct T *t;
    *(vu16 *)0x04000208 = 0;
    ie = *(vu16 *)0x04000200;
    *(vu16 *)0x04000200 = 0;
    t = (struct T *)0x03003A10;
    t->b = sub_082141BC;
    *(vu16 *)0x04000128 &= 0xBFFF;
    *(vu16 *)0x04000200 = ie & 0xFF7F;
    *(vu16 *)0x04000208 = 1;
    *(vu16 *)0x04000202 = 0x80;
}
