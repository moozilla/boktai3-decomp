#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0802048C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_0801F93C(void);
void sub_080203D4(void);
void *sub_0802091C(void *a, void *b) {
    void *obj;
    obj = sub_08219FBC(9, 0x434);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0801F93C, sub_080203D4);
        if (sub_0802048C(obj, a, b) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
