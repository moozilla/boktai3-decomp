#include "global.h"
#include "background_controller.h"

/* Clears the four ScrollingBackground slots in gUnk_02000010. Not listed in
 * symbols/functions.csv; it sits between two units as raw bytes in gen/code.s,
 * so it is decompiled here to keep the subsystem contiguous. */
void sub_08002284(void)
{
    u32 *base = (u32 *)gUnk_02000010;
    u32 zero = 0;
    u32 *p = base + 3;
    do {
        *p-- = zero;
    } while ((s32)p >= (s32)base);
}
