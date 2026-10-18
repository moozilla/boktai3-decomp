#include "global.h"
static inline void st(void (*v)(u8 *), void (**a)(u8 *))
{
    *a = v;
}
void Registry1909_UpdateHiddenState(u8 *p)
{
    *(u32 *)(p + 0x20) |= 1;
    st(Registry1909_UpdateHiddenState, (void (**)(u8 *))(p + 0xd4));
    *(u32 *)(p + 0xd8) = 0;
}
