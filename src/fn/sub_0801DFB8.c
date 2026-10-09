#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0801DEF4(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_0801DE98(void);
void sub_0801CF60(void);
void *sub_0801DFB8(void *a, void *b) {
    void *obj;
    obj = sub_08219FBC(11, 0x7F4);
    if (obj != 0) {
        sub_0821A04C(obj, sub_0801DE98, sub_0801CF60);
        if (sub_0801DEF4(obj, a, b) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
