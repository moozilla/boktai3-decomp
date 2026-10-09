#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0800F220(void *, u16);
void sub_0821A0C0(void *);
void sub_0800F154(void);
void sub_0800F1BC(void);
void *sub_0800F474(u32 arg) {
    void *obj;
    obj = sub_08219FBC(9, 0x308);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0800F154, sub_0800F1BC);
        if (sub_0800F220(obj, arg) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
