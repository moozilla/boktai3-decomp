#include "global.h"
int Text_ParseDecimal(const u8 *text, int length)
{
    int value = 0;
    int power = 1;
    int i;

    for (i = 0; i < length; i++) {
        value += (text[length - i - 1] - '0') * power;
        power *= 10;
    }
    return value;
}
