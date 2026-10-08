#include "global.h"

void sub_0821FE6C(u8 *);
void sub_08013684(u8 *);
u32 sub_08214514(u8 *);

u32 sub_08042B04(u8 *unused, u8 *p) {
    sub_0821FE6C(p + 0x80);
    sub_08013684(p + 0x12c);
    return sub_08214514(p + 0xe4);
}
