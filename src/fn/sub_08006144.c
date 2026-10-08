#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08006134(void *, void *);
void sub_0821A0C0(void *);
void sub_0800611C(void);
void sub_08006120(void);

void *sub_08006144(void *a)
{
    void *p = sub_08219FBC(8, 0x1c);
    if (p != 0) {
        sub_0821A04C(p, sub_0800611C, sub_08006120);
        if (sub_08006134(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
