#include "global.h"

void Dir_Decompose(u8 n, u8 *a, u8 *b, u32 scale)
{
    if (n > 6) {
        u8 m = n - 7;

        if (m > 4) {
            *a = 8 - m;
            *b = 1;
        } else {
            *a = m;
            *b = 0;
        }
    } else if (n > 2) {
        u8 m = n - 3;

        if (m == 1 || m == 2)
            *a = 1;
        else
            *a = 0;
        if (m > 1)
            *b = 1;
        else
            *b = 0;
    } else if (n != 0) {
        if (n == 1) {
            *a = 0;
            *b = 0;
        } else {
            *a = 0;
            *b = 1;
        }
    } else {
        *a = 0;
        *b = 0;
    }
    *a = scale * *a;
}
