#include "global.h"

void sub_08222CA0(void);

void sub_08223400(void) {
    s32 v = *(s32 *)0x03005324;
    if (v >= 0 || v <= -3) {
        if (v == 0)
            sub_08222CA0();
    }
}
