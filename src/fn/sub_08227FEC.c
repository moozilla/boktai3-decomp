// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
void sub_08227FEC(void)
{
    register u32 *guard asm("r1") = &gHeapEndGuard;
    if (*guard != 0x516B92EE)
        *guard = 0x516B92EE;
}
