#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, u32, u32);
void sub_0818EBF4(u8 *);
u32 sub_081906E4(u8 *);
u32 sub_081906F4(u8 *);

u8 *sub_08190704(u32 a, u32 b) {
    u8 *r4 = sub_08219FBC(8, 0x53CC);
    if (r4 != 0) {
        sub_0821A04C(r4, (u32)sub_081906E4, (u32)sub_081906F4);
        sub_0818EBF4(r4 + 0x18);
        *(u16 *)(r4 + 0x53C8) = a;
        *(u16 *)(r4 + 0x53CA) = b;
    }
    return r4;
}
