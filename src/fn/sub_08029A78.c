#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
void sub_08029A4C(void *, int, int);
void ContactSequencePool_SpawnActor(void *, void *, int);
void sub_08029A78(u8 *p, int x, int z)
{
    struct Vec pos;
    sub_08029A4C(&pos, x, z);
    ContactSequencePool_SpawnActor(*(void **)(p + 0x904), &pos, x + z * 3);
}
