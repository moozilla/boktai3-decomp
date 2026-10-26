#include "global.h"
#include "ram.h"
void sub_08237280(void *);
u32 RisingPuffPool_Init(u8 *s)
{
    sub_08237280(s);
    *(u16 *)(s + 0x198) = 0;
    gRisingSpritePool = s;
    return 0;
}
