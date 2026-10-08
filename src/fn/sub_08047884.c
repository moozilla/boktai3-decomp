#include "global.h"

extern u16 gUnk_03004BD8;
u32 sub_08047700(void);
void sub_08047884(void)
{
    if (sub_08047700()) {
        u16 *g = &gUnk_03004BD8;
        u32 m = 0xFFFFFDFF;
        *g = *g & m;
    }
}
