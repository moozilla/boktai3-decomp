#define Libgcc_CallViaR3 sub_08249244
#include "global.h"

/* Tentative C interface; the assembly thunk preserves all argument registers. */
u32 sub_08249244(u32 first, u32 second, u32 third, u32 (*callback)(u32, u32, u32))
{
    return callback(first, second, third);
}
