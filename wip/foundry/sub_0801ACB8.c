#include "global.h"
struct K {
    u16 held;
    u16 pressed;
};
extern struct K gUnk_03005260;

struct S {
    u8 pad[0x1d];
    u8 f1d;
    u8 pad2;
    u8 f1f;
};

u32 sub_0801ACB8(struct S *p)
{
    u32 k;
    u32 pos = p->f1d;
    u32 dir = 0;
    k = gUnk_03005260.pressed;
    if (0x40 & k) {
        dir = 1;
    } else if (0x80 & k) {
        dir = 2;
    } else if (0x20 & k) {
        dir = 4;
    } else {
        if (0x10 & k)
            dir = 8;
        if (dir == 0) {
            u32 c = p->f1f;
            if (c > 7) {
                u32 h = gUnk_03005260.held;
                if (0x40 & h)
                    dir = 1;
                else if (0x80 & h)
                    dir = 2;
                h = gUnk_03005260.held;
                if (0x20 & h)
                    dir |= 4;
                else if (0x10 & h)
                    dir |= 8;
            } else if (0xf0 & gUnk_03005260.held) {
                p->f1f = c + 1;
            }
        }
    }
    if (dir & 1) {
        if ((pos >> 2) == 0)
            pos = (pos & 3) + 12;
        else
            pos -= 4;
    } else if (dir & 2) {
        if ((pos >> 2) == 3)
            pos &= 3;
        else
            pos += 4;
    } else if (dir & 4) {
        if ((pos & 3) == 0)
            pos = (pos & 0xfc) + 3;
        else
            pos -= 1;
    } else if (dir & 8) {
        if ((pos & 3) == 3)
            pos &= 0xfc;
        else
            pos += 1;
    } else {
        return pos;
    }
    p->f1f = 0;
    return pos;
}
