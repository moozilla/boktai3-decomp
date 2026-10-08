#include "global.h"
void sub_0822B2F8(u32);
void sub_0807827C(u8 *p)
{
    u32 m = 6;
    if (*(u16 *)(p + 0x2d4) & m) {
        sub_0822B2F8(0x13e);
        return;
    }
    switch (p[0x98]) {
    case 4:
        sub_0822B2F8(0xd4);
        break;
    case 1:
        sub_0822B2F8(0xfd);
        break;
    case 3:
        switch (*(u16 *)(p + 0x1e6)) {
        case 0:
        case 1:
        case 3:
        case 5:
            sub_0822B2F8(0x18d);
            break;
        }
        break;
    case 0xe:
        sub_0822B2F8(0x1dc);
        break;
    case 0x19:
        sub_0822B2F8(0x1cb);
        break;
    }
}
