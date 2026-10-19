#include "global.h"

typedef struct { u16 x; u16 pad; u16 y; u16 pad2; } P;
void sub_081263E0(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

#define ip ((u32 *)0x03005308)
#define STEP() (idx = (idx + 1) & m1)
#define STEPS() (idx = (idx + 1) & m1, *ip = idx)

void sub_08107398(u8 *p)
{
    u32 f = *(u32 *)0x030053F4 & 0x800;
    P loc;
    P *src;
    if (f != 0) return;
    {
        int v = *(u16 *)(p + 0xb8);
        if (!(v <= 1)) return;
        if (!(v >= 0)) return;
    }
    src = (P *)(p + 0x84);
    loc = *src;
  {
    u16 *tbl = (u16 *)0x0203B400;
    u32 m1 = 0x3FF;
    u32 m2 = 0x3F;
    u32 idx;
    u16 t;
    idx = *ip;
    STEP(); t = tbl[idx];
    { u32 w = loc.x + 0x5c; loc.x = w - (t & m2); }
    STEPS(); t = tbl[idx];
    { u32 w = loc.y + 0x5c; loc.y = w - (t & m2); }
    sub_081263E0(&loc, 8, 8, 4, 4, f, f, 0x20, 0x40, 1, 1, 0x200);
    loc = *src;
    idx = *ip;
    STEP(); t = tbl[idx];
    { u32 w = loc.x - 0x5c; loc.x = w - (t & m2); }
    STEPS(); t = tbl[idx];
    { u32 w = loc.y - 0x5c; loc.y = w - (t & m2); }
    sub_081263E0(&loc, 8, 8, 4, 4, f, f, 0x20, 0x40, 1, 1, 0x200);
    loc = *src;
    idx = *ip;
    STEP(); t = tbl[idx];
    { u32 w = loc.x - 0x5c; loc.x = w - (t & m2); }
    STEPS(); t = tbl[idx];
    { u32 w = loc.y + 0x5c; loc.y = w - (t & m2); }
    sub_081263E0(&loc, 8, 8, 4, 4, f, f, 0x20, 0x40, 1, 1, 0x200);
    loc = *src;
    idx = *ip;
    STEP(); t = tbl[idx];
    { u32 w = loc.x + 0x5c; loc.x = w - (t & m2); }
    STEPS(); t = tbl[idx];
    { u32 w = loc.y - 0x5c; loc.y = w - (t & m2); }
    sub_081263E0(&loc, 8, 8, 4, 2, f, f, 0x20, 0x40, 1, 1, 0x200);
    loc = *src;
    idx = *ip;
    STEPS(); t = tbl[idx];
    { u32 w = loc.x - 0x5c; loc.x = w - (t & m2); }
    sub_081263E0(&loc, 8, 8, 4, 2, f, f, 0x20, 0x40, 1, 1, 0x200);
    loc = *src;
    idx = *ip;
    STEPS(); t = tbl[idx];
    { u32 w = loc.x + 0x5c; loc.x = w - (t & m2); }
    sub_081263E0(&loc, 8, 8, 4, 4, f, f, 0x20, 0x40, 1, 1, 0x200);
  }
}
