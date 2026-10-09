#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08011118(void *, void *);
void sub_0821A0C0(void *);
void sub_08010FD4(void);
void sub_080110E8(void);
void *sub_080111A8(void *arg) {
    void *obj;
    obj = sub_08219FBC(11, 0xD08);
    if (obj != 0) {
        sub_0821A04C(obj, sub_08010FD4, sub_080110E8);
        if (sub_08011118(obj, arg) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
