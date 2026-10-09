#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08018E00(void *);
void sub_0821A0C0(void *);
void sub_08018D94(void);
void sub_08018DFC(void);
void *sub_08018EF0(void) {
    void *obj;
    obj = sub_08219FBC(11, 0x2C);
    if (obj != 0) {
        sub_0821A04C(obj, sub_08018D94, sub_08018DFC);
        if (sub_08018E00(obj) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
