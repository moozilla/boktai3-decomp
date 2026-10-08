#include "global.h"

struct T { u32 a; void (*b)(void); void (*c)(void); };
void sub_082227D8(void);

void sub_08222730(void) {
    u16 ie;
    struct T *t;
    *(vu16 *)0x04000208 = 0;
    ie = *(vu16 *)0x04000200;
    *(vu16 *)0x04000200 = 0;
    t = (struct T *)0x03003A10;
    t->b = sub_082227D8;
    *(vu16 *)0x04000128 |= 0x4000;
    ie |= 0x80;
    *(vu16 *)0x04000200 = ie;
    *(vu16 *)0x04000208 = 1;
    *(vu16 *)0x04000202 = 0x80;
}
