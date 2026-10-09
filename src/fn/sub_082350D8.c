#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(u8 *), void (*)(u8 *));
void sub_0821A0C0(u8 *);
void sub_08234ECC(u8 *);
void sub_08234EF4(u8 *);
s32 sub_08235000(u8 *, u8 *);
u8 *sub_082350D8(u8 *p)
{
    u8 *obj = sub_08219FBC(11, 0x278);
    if (obj) {
        sub_0821A04C(obj, sub_08234ECC, sub_08234EF4);
        if (sub_08235000(obj, p) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
