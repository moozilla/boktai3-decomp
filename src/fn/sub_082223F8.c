#include "global.h"

void sub_08219D38(u32);

void sub_082223F8(u32 *p) {
    if (p[1] != 0) {
        sub_08219D38(p[1]);
        p[1] = 0;
    }
}
