#include "global.h"

u8 *sub_0822590C(u8 *m) {
    u8 *s = *(u8 **)0x030025F8;
    if (s == 0)
        return 0;
    {
        u8 *n = *(u8 **)(s + 0x18);
        if (n != 0) {
            do {
                if (n == m)
                    return n;
                n = *(u8 **)(n + 0x44);
            } while (n != 0);
        }
    }
    return 0;
}
