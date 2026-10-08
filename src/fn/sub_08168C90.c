#include "global.h"
extern u32 gUnk_0300523C;
void sub_081680F4(u8 *);
void sub_08168214(u8 *);
void sub_0822B500(void);
void sub_08163EA0(u8 *, void *);
void sub_08169228(void);
static inline void orm2(u32 m, u32 *a) { *a |= m; }
void sub_08168C90(u8 *p)
{
    sub_081680F4(p);
    sub_08168214(p);
    sub_0822B500();
    orm2(4, &gUnk_0300523C);
    sub_08163EA0(p, sub_08169228);
}
