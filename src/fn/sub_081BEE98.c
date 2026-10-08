#include "global.h"
s32 sub_0805E538(u8 *);
void sub_081BECF0(u8 *);
void sub_081BEE48(u8 *);
void sub_081BEE98(u8 *p)
{
    if (sub_0805E538(p + 0x1558)) {
        sub_081BECF0(p);
        sub_081BEE48(p);
    }
}
