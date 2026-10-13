#include "global.h"

u32 rfu_syncVBlank(void);
void sub_08224980(u32, u32);
void sub_08224C84(void);

void sub_08223BA4(void)
{
    if ((u16)rfu_syncVBlank() != 0) {
        sub_08224980(0xf1, 0);
        sub_08224C84();
    }
}
