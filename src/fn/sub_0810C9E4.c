#include "global.h"

struct P { u8 f[0xf40]; void *cb; };
s32 sub_08109464(void);
void sub_080335B4(void);
void sub_0821A0C0(void *);
void sub_0824923C(void *, void *);

s32 sub_0810C9E4(struct P *p)
{
    if (sub_08109464() == 0) {
        sub_080335B4();
        sub_0821A0C0(p);
        return -1;
    }
    if (p->cb != 0)
        sub_0824923C(p, p->cb);
    return 1;
}
