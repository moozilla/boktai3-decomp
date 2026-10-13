#include "global.h"
/* Create the singleton animated controller and install its task callbacks. */
struct S {
    u8 pad[0xf8a];
    u16 resource;
};
extern struct S *gUnk_020003C8;
struct S *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
void sub_081E9D84(void *);
void sub_081E9DE4(void *);
s32 sub_081EA4DC(void *);
void sub_0821A0C0(void *);
struct S *sub_081EA730(u32 resource, u32 unused)
{
    struct S *p;
    if (gUnk_020003C8)
        return gUnk_020003C8;
    p = sub_08219FBC(11, 0xfa4);
    if (p) {
        p->resource = resource;
        sub_0821A04C(p, sub_081E9D84, sub_081E9DE4);
        if (sub_081EA4DC(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
