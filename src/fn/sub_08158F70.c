#include "global.h"
#include "ram.h"

struct O08158F70 {
    u32 f0;
    u32 f4;
    u32 f8;
};
void sub_08249244(u32, u32, u32, u32);

void Player_NotifyActionInterruption(u8 *p)
{
    struct O08158F70 *o = gPlayerPendingActor;
    if (o != 0 && p[0x458] == 2 && p[0x457] == 0xc) {
        if (o->f4 & 0x10)
            sub_08249244(o->f0, 1, 0x3c, o->f8);
        else
            sub_08249244(o->f0, 1, 0x3c, o->f8);
    }
}
