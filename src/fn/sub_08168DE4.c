#include "global.h"
extern u32 gUnk_0300523C;
void sub_081681B8(u8 *);
void sub_08168D20(u8 *);
void sub_08168CC4(u8 *);
void sub_08168DB0(u8 *);
void sub_081685B4(u8 *);
void sub_0822B524(void);
void sub_08163EA0(u8 *, void *);
void sub_08167C84(void);
static inline void andm(u32 *a, u32 m) { *a &= m; }
void sub_08168DE4(u8 *p)
{
    sub_081681B8(p);
    sub_08168D20(p);
    sub_08168CC4(p);
    sub_08168DB0(p);
    p[0xC73] = 0;
    sub_081685B4(p);
    sub_0822B524();
    andm(&gUnk_0300523C, ~4);
    sub_08163EA0(p, sub_08167C84);
}
