#include "global.h"
int Text_ParseDecimal(const u8 *p, int n)
{
    int value = 0, power = 1;
    if (value < n) {
        const u8 *q;
        p--;
        q = (const u8 *)((u32)n + (u32)p);
        do {
            value += (*q - 48) * power;
            power *= 10;
            q--;
            n--;
        } while (n);
    }
    return value;
}
