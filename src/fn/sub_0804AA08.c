#include "global.h"
struct Obj {
    u8 pad0[0x43c];
    u16 f43c;
    u16 f43e;
    u8 pad442[0];
};
void sub_0804AA08(u8 *p)
{
    u16 zero;
    u16 two;
    u16 *first;
    first = (u16 *)(p + 0x43c);
    zero = 0;
    two = 2;
    *first = two;
    *(u16 *)(p + 0x43e) = zero;
    *(u16 *)(p + 0x442) = 0x40;
    *(u16 *)(p + 0x444) = 3;
    *(u16 *)(p + 0x446) = 6;
    *(u16 *)(p + 0x448) = two;
    *(u16 *)(p + 0x44a) = two;
    *(u16 *)(p + 0x44c) = 0x3c;
    *(u16 *)(p + 0x44e) = 8;
    *(u16 *)(p + 0x450) = zero;
    *(u16 *)(p + 0x452) = 0xff;
    *(u16 *)(p + 0x454) = 0xff;
    *(u16 *)(p + 0x456) = zero;
    *(u16 *)(p + 0x458) = 0xff;
    *(u16 *)(p + 0x45a) = zero;
    *(u16 *)(p + 0x45c) = 0xff;
    *(u16 *)(p + 0x45e) = zero;
    *(u16 *)(p + 0x460) = 0xff;
    *(u16 *)(p + 0x462) = zero;
}
