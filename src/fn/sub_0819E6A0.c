#include "global.h"
struct P { u8 f0[0xad]; u8 i; };
extern const u32 gUnk_08611E08[];
void sub_0824923C(void *, u32);
void sub_0819E408(void *);
void sub_0819E6A0(struct P *p)
{
    sub_0824923C(p, gUnk_08611E08[p->i]);
    sub_0819E408(p);
}
