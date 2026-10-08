#include "global.h"

void sub_08195774(u8 *);

u8 sub_081957E0(u8 *p) {
    u8 *q = p + 0x725E;
    u8 *t;
    u8 v;
    u8 r;
    if (*q > 8) {
        sub_08195774(p);
    }
    v = *q;
    t = p + 0x725F;
    r = t[v];
    *q = v + 1;
    return r;
}
