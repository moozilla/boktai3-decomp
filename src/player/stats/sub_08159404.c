#include "global.h"

struct P08159404 {
    u8 f0[0x474];
    u16 f474;
    u16 f476;
};

void Player_AddAttachmentFlags(struct P08159404 *p, u32 a, u16 b)
{
    p->f474 |= a;
    p->f476 = b;
}
