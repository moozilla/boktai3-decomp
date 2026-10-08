#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0822B8EC(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_0822B8D4(void);
void sub_0822B8E0(void);

void *sub_0822B924(u32 a, u32 b) {
    void *p = sub_08219FBC(5, 0xD8);
    if (p) {
        sub_0821A04C(p, sub_0822B8D4, sub_0822B8E0);
        if (sub_0822B8EC(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
