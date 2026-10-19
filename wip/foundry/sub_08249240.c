#define Libgcc_CallViaR2 sub_08249240
#include "global.h"

/* Tentative C interface; the assembly thunk preserves all argument registers. */
u32 sub_08249240(u32 first, u32 second, u32 (*callback)(u32, u32))
{
    return callback(first, second);
}
