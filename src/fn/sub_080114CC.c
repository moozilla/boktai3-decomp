#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0801140C(void *);
void sub_0821A0C0(void *);
void sub_0801138C(void);
void sub_080113E0(void);
void *sub_080114CC(void) {
    void *obj;
    obj = sub_08219FBC(11, 0x4D8);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0801138C, sub_080113E0);
        if (sub_0801140C(obj) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
