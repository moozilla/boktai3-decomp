#include "global.h"
#define SceneObjects_BeginActivation sub_0801E530
void sub_0801EFDC(void);
void SceneObjects_BeginActivation(u8 *p, u16 id)
{
    int i = 0;
    if (i < *(u16 *)(p + 0x1a)) {
        u8 **base = (u8 **)(p + 0x42c);
        int two;
        two = 2;
        do {
            u8 *s = *base + 140 * i;
            if (*(u16 *)(s + 0x34) == id && (s[0x37] == 0 || s[0x37] == 3) && !(s[0x36] & 1)) {
                s[0x41] = 1;
                s[0x37] = two;
                *(void **)(s + 0x30) = sub_0801EFDC;
                s[0x39] = 0;
                s[0x3a] = 0;
            }
            i++;
        } while (i < *(u16 *)(p + 0x1a));
    }
}
