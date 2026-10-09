#include "global.h"
struct Pair47 { u32 a, b; };
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(u8 *), void (*)(u8 *));
void sub_0821A0C0(u8 *);
void sub_0823450C(u8 *);
void sub_08234520(u8 *);
s32 sub_08234770(u8 *, u8 *, struct Pair47 *);
u8 *sub_082347C8(u8 *p, struct Pair47 *q)
{
    u8 *obj = sub_08219FBC(8, 0x208);
    if (obj) {
        sub_0821A04C(obj, sub_0823450C, sub_08234520);
        if (sub_08234770(obj, p, q) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
