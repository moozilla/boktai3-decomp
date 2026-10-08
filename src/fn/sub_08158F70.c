#include "global.h"

struct O08158F70 { u32 f0; u32 f4; u32 f8; };
extern struct O08158F70 *gUnk_0200054C;
void sub_08249244(u32, u32, u32, u32);

void sub_08158F70(u8 *p)
{
    struct O08158F70 *o = gUnk_0200054C;
    if (o != 0 && p[0x458] == 2 && p[0x457] == 0xc) {
        if (o->f4 & 0x10)
            sub_08249244(o->f0, 1, 0x3c, o->f8);
        else
            sub_08249244(o->f0, 1, 0x3c, o->f8);
    }
}
