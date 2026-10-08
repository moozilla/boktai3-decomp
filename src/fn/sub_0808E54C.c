#include "global.h"
void sub_08076818(u8 *, u32);
void sub_0808E54C(u8 *p)
{
    if (p[0x2b2] != 0) p[0x2b2] = 0;
    sub_08076818(p, 1);
}
