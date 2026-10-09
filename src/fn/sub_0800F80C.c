#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0800F6F8(void *, u16);
void sub_0821A0C0(void *);
void sub_0800F6E0(void);
void sub_0800F6EC(void);
void *sub_0800F80C(u32 arg) {
    void *obj;
    obj = sub_08219FBC(9, 0x2C);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0800F6E0, sub_0800F6EC);
        if (sub_0800F6F8(obj, arg) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
