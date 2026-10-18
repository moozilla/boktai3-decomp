#include "global.h"

void InterruptsAndVideo_OnUnusedInterrupt(void);

struct T {
    u32 a;
    u32 b;
    void (*f)(void);
};

void Serial_DisableTimer3Polling(void)
{
    u16 ie;
    struct T *t;
    *(vu16 *)0x04000208 = 0;
    ie = *(vu16 *)0x04000200;
    *(vu16 *)0x04000200 = 0;
    *(vu16 *)0x0400010E = 0;
    *(vu16 *)0x0400010C = 0;
    t = (struct T *)0x03003A10;
    t->f = InterruptsAndVideo_OnUnusedInterrupt;
    *(vu16 *)0x04000200 = ie & 0xFFBF;
    *(vu16 *)0x04000208 = 1;
    *(vu16 *)0x04000202 = 0x40;
}
