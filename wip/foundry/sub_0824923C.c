#define Libgcc_CallViaR1 sub_0824923C
#include "global.h"

/* Tentative C interface: this runtime thunk forwards r0 through r1. */
u32 sub_0824923C(u32 argument, u32 (*callback)(u32))
{
    return callback(argument);
}
