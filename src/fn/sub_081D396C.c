#include "global.h"

#define Dir_Decompose sub_081D396C

void Dir_Decompose(u8 n, u8 *a, u8 *b, u32 scale)
{
    u8 m;
    if (n > 6) {
        m = n - 7;
        if (m > 4) {
            *a = 8 - m;
            goto setB1;
        }
        *a = m;
        goto setB0;
    } else if (n > 2) {
        m = n - 3;
        if ((u8)(m - 1) <= 1)
            *a = 1;
        else
            *a = 0;
        if (m > 1)
            goto setB1;
    setB0:
        *b = 0;
    } else if (n != 0) {
        if (n == 1) {
            *a = 0;
            *b = 0;
        } else {
            *a = 0;
        setB1:
            *b = 1;
        }
    } else {
        *a = n;
        *b = n;
    }
    *a = scale * *a;
}
