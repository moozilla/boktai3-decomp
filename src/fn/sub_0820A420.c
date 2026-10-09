#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0820A3AC(void *);
void sub_0821A0C0(void *);
void sub_0820A2C8(void);
void sub_0820A364(void);

void *sub_0820A420(void) {
    void *p = sub_08219FBC(8, 0xc1c);
    if (p) {
        sub_0821A04C(p, sub_0820A2C8, sub_0820A364);
        if (sub_0820A3AC(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
        return p;
    }
}
