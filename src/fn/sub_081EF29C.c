#include "global.h"
void sub_081EE518(void);
struct S { u8 f0[0x90]; u32 a90; u8 f1[0xAE - 0x94]; u8 aae; u8 af; u8 b0; u8 f2[0xC4-0xB1]; u32 c4; u8 f3[0x100-0xC8]; void (*fn)(void); };
static inline u8 chk(struct S *p)
{
    u8 *q = &p->b0;
    if (*q != 0) {
        *q = 0;
        *(q - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
static inline void st(void (*v)(void), void (**a)(void)) { *a = v; }
void sub_081EF29C(struct S *p)
{
    u8 *q;
    u32 r;
    if (chk(p) == 1) {
        st(sub_081EE518, &p->fn);
        p->a90 = 0;
        *((u8 *)p + 0xb1) = 1;
        *((u8 *)p + 0xa2) = 0;
        *(u32 *)((u8 *)p + 0xa4) = 0;
    }
    p->c4++;
}
