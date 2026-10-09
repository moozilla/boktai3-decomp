#include "global.h"
struct Pair3B { u32 a, b; };
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(u8 *), void (*)(u8 *));
void sub_0821A0C0(u8 *);
void sub_082338EC(u8 *);
void sub_08233900(u8 *);
s32 sub_08233AA0(u8 *, struct Pair3B *, u32, u32, u32, u32);
u8 *sub_08233B00(struct Pair3B *p, u32 a, u32 b, u32 c, u32 d)
{
    u8 *obj = sub_08219FBC(8, 0x210);
    if (obj) {
        sub_0821A04C(obj, sub_082338EC, sub_08233900);
        if (sub_08233AA0(obj, p, a, b, c, d) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
