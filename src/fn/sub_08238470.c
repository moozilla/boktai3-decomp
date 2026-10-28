#include "global.h"
#include "area_effect_link.h"
struct S {
    u8 p[0x18];
    s16 id;
};
void Sound_PlaySong(u32);
static inline s32 gid(void)
{
    if (((struct S *)gLinkSession) == 0)
        return -1;
    return ((struct S *)gLinkSession)->id;
}
void Player_PlayLocalSong(s32 a, u32 b)
{
    if (a == gid())
        Sound_PlaySong(b);
}
