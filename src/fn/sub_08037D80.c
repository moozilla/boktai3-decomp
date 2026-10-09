#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08037BB8(void *);
void sub_0821A0C0(void *);
void sub_08037B20(void);
void sub_08037B6C(void);
void *sub_08037D80(void) {
    void *obj;
    obj = sub_08219FBC(11, 0x694);
    if (obj != 0) {
        sub_0821A04C(obj, sub_08037B20, sub_08037B6C);
        if (sub_08037BB8(obj) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
