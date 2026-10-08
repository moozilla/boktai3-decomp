#include "global.h"

void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
s32 sub_0808D1D8(void *);
void sub_0808D1CC(void *);

void sub_0808E1D4(void)
{
    void *m = sub_08219C40(0x678);
    if (m != 0) {
        sub_08219DD8(m, 0x678);
        if (sub_0808D1D8(m) < 0) {
            sub_0808D1CC(m);
            sub_08219D38(m);
        }
    }
}
