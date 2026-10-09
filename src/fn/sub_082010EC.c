#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0820107C(void *);
void sub_0821A0C0(void *);
void sub_08200C94(void);
void sub_08201024(void);

void *sub_082010EC(void) {
    void *p = sub_08219FBC(8, 0xa2c);
    if (p) {
        sub_0821A04C(p, sub_08200C94, sub_08201024);
        if (sub_0820107C(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
        return p;
    }
}
