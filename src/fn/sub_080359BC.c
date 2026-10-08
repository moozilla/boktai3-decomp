#include "global.h"

extern u32 gUnk_02000484;
void sub_0824693C(void);
void sub_082456E0(void);
void sub_0824780C(void);
void sub_080359BC(u16 v)
{
    if (gUnk_02000484 != 0) {
        if (v == 0x27) {
            sub_0824693C();
            sub_082456E0();
        } else
            sub_0824780C();
    }
}
