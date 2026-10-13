#include "global.h"
struct Vm {
    u8 pad[0x88];
    s32 numbers[16];
};
int sub_08032D10(struct Vm *p, int index)
{
    int value = p->numbers[index];
    u32 n = (u32)value >> 31;
    int abs = value;
    if (value < 0)
        abs = -value;
    value = abs;
    if (value > 9999999)
        n += 8;
    else if (value > 999999)
        n += 7;
    else if (value > 99999)
        n += 6;
    else if (value > 9999)
        n += 5;
    else if (value > 999)
        n += 4;
    else if (value > 99)
        n += 3;
    else if (value > 9)
        n += 2;
    else
        n++;
    return n;
}
