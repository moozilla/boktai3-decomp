#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0800E22C(void *, u16);
void sub_0821A0C0(void *);
void sub_0800E130(void);
void sub_0800E228(void);
void *sub_0800E390(u32 arg) {
    void *obj;
    obj = sub_08219FBC(12, 0x2C8);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0800E130, sub_0800E228);
        if (sub_0800E22C(obj, arg) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
