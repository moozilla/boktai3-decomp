#include "global.h"
void sub_08138154(void *, u32, u32);
void sub_08055B74(void);
void sub_08223270(void);
void sub_08138380(u8 *p, u32 b)
{
    u8 t;
    *(u16 *)(p + 0x1a) = 0;
    t = p[0x1c];
    p[0x20] = t;
    sub_08138154(p, 0, b);
    sub_08055B74();
    sub_08223270();
}
