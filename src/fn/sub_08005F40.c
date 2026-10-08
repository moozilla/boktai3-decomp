#include "global.h"

struct N { u16 id; u8 f2; u8 pad; u32 a[2]; u8 sub[1]; };
extern u32 gUnk_02000028;
void sub_0821D6D0(void *);
void sub_08005E40(void *);
void sub_08219D38(void *);

u32 sub_08005F40(struct N *n)
{
    if (n->f2 != 0) {
        sub_0821D6D0(n->sub);
        n->f2 = 0;
    }
    if (gUnk_02000028 != 0)
        sub_08005E40(n);
    sub_08219D38(n);
    return 0;
}
