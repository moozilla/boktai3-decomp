#include "global.h"

struct A {
    u8 f[0x15];
    u8 a;
    u16 b;
};
void Sound_StopSong(s32);

void sub_081A68CC(struct A *p)
{
    p->b = 0;
    p->a = 0;
    Sound_StopSong(0x583);
}
