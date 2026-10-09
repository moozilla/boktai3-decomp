#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0800C964(void *, void *);
void sub_0821A0C0(void *);
void sub_0800C878(void);
void sub_0800C954(void);
void *sub_0800CB60(void *arg) {
    void *obj;
    obj = sub_08219FBC(8, 0x90);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0800C878, sub_0800C954);
        if (sub_0800C964(obj, arg) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
