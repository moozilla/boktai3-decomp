#include "global.h"

struct S0813B154 {
    u8 filler[0x418];
    u8 v;
};
void Sound_StopSong(u32);

void sub_0813B154(struct S0813B154 *p)
{
    if (p->v != 6) {
        Sound_StopSong(0xd8);
    } else {
        Sound_StopSong(0x239);
        Sound_StopSong(0x202);
        Sound_StopSong(0x366);
    }
}
