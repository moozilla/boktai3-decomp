#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08013294(void *);
void sub_0821A0C0(void *);
void sub_0801324C(void);
void sub_08013270(void);
void *sub_08013380(void) {
    void *obj;
    obj = sub_08219FBC(8, 0x2FC);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0801324C, sub_08013270);
        if (sub_08013294(obj) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
