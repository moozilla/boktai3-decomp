#define Libgcc_CallViaR4 sub_08249248
#include "global.h"

/* Semantic forwarding draft only: C passes callback on the stack,
 * whereas the real assembly entry receives it in r4. */
u32 sub_08249248(u32 first, u32 second, u32 third, u32 fourth,
                 u32 (*callback)(u32, u32, u32, u32))
{
    return callback(first, second, third, fourth);
}
