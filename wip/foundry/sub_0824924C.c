#define Libgcc_CallViaR5 sub_0824924C
#include "global.h"

/* Semantic forwarding draft only: C passes callback on the stack,
 * whereas the real assembly entry receives it in r5.
 * Subsequent retained bytes contain separate call-via-register thunks. */
u32 sub_0824924C(u32 first, u32 second, u32 third, u32 fourth,
                 u32 (*callback)(u32, u32, u32, u32))
{
    return callback(first, second, third, fourth);
}
