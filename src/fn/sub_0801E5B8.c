#include "global.h"
#define SceneObjects_BeginDeactivation sub_0801E5B8
void sub_0801EF60(void);
void SceneObjects_BeginDeactivation(u8 *p, u16 id)
{
    int i = 0;
    if (i < *(u16 *)(p + 0x1a)) {
        u8 **base = (u8 **)(p + 0x42c);
        int three;
        int zero;
        int two;
        three = 3;
        zero = i;
        two = 2;
        do {
            u8 *s = *base + 140 * i;
            if (*(u16 *)(s + 0x34) == id && (u8)(s[0x37] - 1) <= 1) {
                s[0x37] = three;
                *(void **)(s + 0x30) = sub_0801EF60;
                s[0x39] = two;
                s[0x3a] = zero;
            }
            i++;
        } while (i < *(u16 *)(p + 0x1a));
    }
}
