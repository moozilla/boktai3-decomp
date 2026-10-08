#include "global.h"
static inline void st(void (*v)(u8 *), void (**a)(u8 *)) { *a = v; }
void sub_081C5120(u8 *p)
{
    *(u32 *)(p + 0x20) |= 1;
    st(sub_081C5120, (void (**)(u8 *))(p + 0xd4));
    *(u32 *)(p + 0xd8) = 0;
}
