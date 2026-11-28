#include "global.h"
int Text_ExtractDecimalDigit(int *, int);
void Text_FormatDecimal(u8 *buffer, int value)
{
    int started = 0;
    u8 *out = buffer;
    int digit;
    int i;
    for (i = 0; i < 12; i++)
        buffer[i] = 0;
    if (value < 0) {
        *out = '-';
        out++;
        value = -value;
    }
    digit = Text_ExtractDecimalDigit(&value, 100000);
    if (digit) {
        *out = digit + '0';
        started = 1;
        out++;
    }
    digit = Text_ExtractDecimalDigit(&value, 10000);
    if (digit || started) {
        *out = digit + '0';
        started = 1;
        out++;
    }
    digit = Text_ExtractDecimalDigit(&value, 1000);
    if (digit || started) {
        *out = digit + '0';
        started = 1;
        out++;
    }
    digit = Text_ExtractDecimalDigit(&value, 100);
    if (digit || started) {
        *out = digit + '0';
        started = 1;
        out++;
    }
    digit = Text_ExtractDecimalDigit(&value, 10);
    if (digit || started) {
        *out = digit + '0';
        out++;
    }
    digit = 0;
    while (value > 0) {
        digit++;
        value--;
    }
    *out++ = digit + '0';
    *out = 0;
}
