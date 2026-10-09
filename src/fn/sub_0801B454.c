#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0801B324(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_0801B2D4(void);
void sub_0801B2F8(void);
void *sub_0801B454(void *a, void *b) {
    void *obj;
    obj = sub_08219FBC(11, 0xDB4);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0801B2D4, sub_0801B2F8);
        if (sub_0801B324(obj, a, b) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
