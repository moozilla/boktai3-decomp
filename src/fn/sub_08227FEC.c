// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u32 gUnk_0201F400;
void sub_08227FEC(void)
{
    register u32 *guard asm("r1") = &gUnk_0201F400;
    if (*guard != 0x516B92EE)
        *guard = 0x516B92EE;
}
