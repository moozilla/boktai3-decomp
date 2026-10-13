#include "global.h"
void Script_Call(u32, u32);
void Task_Kill(u8 *);
void sub_081BFE70(u8 *p)
{
    u16 *c = (u16 *)(p + 0x174e);
    (*c)++;
    if (*c > 0x27) {
        u32 v = *(u32 *)(p + 0x17ec);
        if (v)
            Script_Call(v, 0);
        Task_Kill(p);
    }
}
