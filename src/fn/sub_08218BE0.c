#include "global.h"

struct S { u32 a, b, c; };
extern struct S *gUnk_03001664;
struct S *sub_08219C40(u32);
void sub_08219DD8(struct S *, u32);
struct S *sub_0821A520(u32, u32);

s32 sub_08218BE0(void)
{
    if (gUnk_03001664 != 0)
        return 0;
    gUnk_03001664 = sub_08219C40(12);
    if (gUnk_03001664 != 0) {
        struct S *p;
        sub_08219DD8(gUnk_03001664, 12);
        p = sub_0821A520(0xA635, 0x3F51);
        if (p != 0) {
            *gUnk_03001664 = *p;
            gUnk_03001664->b += (u32)p;
            gUnk_03001664->c += (u32)p;
            return 0;
        }
    }
    return -1;
}
