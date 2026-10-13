#include "global.h"
struct S {
    u8 f[0x524];
    u16 b;
    u16 a;
};
void Sound_StopSong(u32);
void Sound_PlaySong(u32);
void sub_0811D03C(struct S *s)
{
    u16 *a = &s->a;
    u16 *b = &s->b;
    if (*a != *b) {
        Sound_StopSong(0x87);
        Sound_StopSong(0x88);
        Sound_StopSong(0x89);
        Sound_StopSong(0x8a);
        Sound_StopSong(0x8b);
        Sound_PlaySong(*b);
        *a = *b;
    }
}
