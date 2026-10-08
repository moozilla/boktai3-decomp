#include "global.h"

struct S { u8 filler[0xd0]; u16 h; u8 filler2[0xe0 - 0xd2]; u32 f; };
void sub_0822B2F8(u32);

void sub_08100A64(struct S *s)
{
    if (!(s->f & 4)) {
        switch (s->h) {
        case 3:
            if (!(s->f & 0x20))
                goto b;
        case 0:
            sub_0822B2F8(0x158);
            break;
        b:
        case 1:
        case 2:
        case 4:
            sub_0822B2F8(0x261);
            break;
        default:
            sub_0822B2F8(0x261);
            break;
        }
    }
}
