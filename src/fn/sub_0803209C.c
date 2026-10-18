#include "global.h"
int Text_ExtractDecimalDigit(int *, int);
void Text_FormatDecimal(u8 *buffer, volatile int value)
{
    int started = 0;
    u8 *out = buffer;
    int digit;
    int zero = 0;
    u8 *q = buffer + 11;
    do {
        *q = zero;
        q--;
    } while ((s32)q >= (s32)buffer);
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
    {
        int v;
        int next;
        u8 *end;
        digit = 0;
        v = value;
        end = out + 1;
        if (v > 0) {
            do {
                digit++;
                next = v - 1;
                v = next;
            } while (next > 0);
            value = next;
        }
        *out = digit + '0';
        *end = 0;
    }
}
