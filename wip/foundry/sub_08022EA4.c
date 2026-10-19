#include "global.h"

void sub_0821EDE0();

struct Pair { u32 a, b; };
struct In { u8 pad[0xC]; u8 f; };

void sub_08022EA4(u8 *p, struct Pair *out)
{
    struct In *in = (struct In *)(p + 0x48);

    switch (*(u16 *)(p + 0x3B2)) {
    case 0:
        sub_0821EDE0(out, &in->f, 0, 0x10000);
        break;
    case 1:
        sub_0821EDE0(out, &in->f, 2, 0x10000);
        break;
    case 2:
        sub_0821EDE0(out, &in->f, 1, 0x10000);
        *(u16 *)(p + 0x354) = 0x20;
        return;
    case 3:
        sub_0821EDE0(out, &in->f, 3, 0x10000);
        break;
    case 4:
        sub_0821EDE0(out, &in->f, 3, 0x10000);
        break;
    case 5:
        sub_0821EDE0(out, &in->f, 3, 0x10000);
        break;
    case 6:
        sub_0821EDE0(out, &in->f, 2, 0x10000);
        break;
    default:
        *out = *(struct Pair *)(p + 0x54);
        break;
    }
    *(u16 *)(p + 0x354) = 0x40;
}
