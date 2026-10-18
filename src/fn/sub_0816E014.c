#define InventoryGrid_ShowNumber sub_0816E014
#include "global.h"
struct InventoryNumberPosition {
    s16 x, y, z, pad;
};
struct InventoryNumberDigit {
    u32 flags;
    u8 unknown04[0x14];
    struct InventoryNumberPosition position;
    u8 unknown20[8];
};
struct InventoryGrid {
    u8 unknown0000[0x4CDC];
    void *digitResource;
    struct InventoryNumberDigit digits[4];
};
s32 Div(s32, s32);
void sub_08217EEC(void *, void *, u32);
void sub_0816E014(struct InventoryGrid *s, s32 value, struct InventoryNumberPosition *origin)
{
    s32 digits[4];
    s32 first, i, remainder;
    struct InventoryNumberPosition position;
    struct InventoryNumberDigit *digit;
    s32 *number;
    if (value > 0) {
        if (value > 9999)
            value = 9999;
        digits[0] = Div(value, 1000);
        remainder = value - digits[0] * 1000;
        digits[1] = Div(remainder, 100);
        remainder -= digits[1] * 100;
        digits[2] = Div(remainder, 10);
        remainder -= digits[2] * 10;
        digits[3] = remainder;
        if (digits[0] > 0)
            first = 0;
        else if (digits[1] > 0)
            first = 1;
        else if (digits[2] > 0)
            first = 2;
        else
            first = 3;
        position = *origin;
        position.x -= (4 - first) * 4;
        position.y -= 4;
        i = 0;
        number = digits;
        digit = s->digits;
        do {
            if (i < first)
                digit->flags |= 1;
            else {
                digit->flags &= ~1;
                sub_08217EEC(&s->digits[i], s->digitResource, *number + 42);
                digit->position = position;
                position.x += 5;
            }
            number++;
            digit++;
        } while (++i <= 3);
    }
}
