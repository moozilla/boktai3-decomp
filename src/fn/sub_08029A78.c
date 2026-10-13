#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
void sub_08029A4C(void *, int, int);
void sub_080466E4(void *, void *, int);
void sub_08029A78(u8 *p, int x, int z)
{
    struct Vec pos;
    sub_08029A4C(&pos, x, z);
    sub_080466E4(*(void **)(p + 0x904), &pos, x + z * 3);
}
