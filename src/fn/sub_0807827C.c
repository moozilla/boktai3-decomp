#include "global.h"
void Sound_PlaySong(u32);
void sub_0807827C(u8 *p)
{
    u32 m = 6;
    if (*(u16 *)(p + 0x2d4) & m) {
        Sound_PlaySong(0x13e);
        return;
    }
    switch (p[0x98]) {
        case 4:
            Sound_PlaySong(0xd4);
            break;
        case 1:
            Sound_PlaySong(0xfd);
            break;
        case 3:
            switch (*(u16 *)(p + 0x1e6)) {
                case 0:
                case 1:
                case 3:
                case 5:
                    Sound_PlaySong(0x18d);
                    break;
            }
            break;
        case 0xe:
            Sound_PlaySong(0x1dc);
            break;
        case 0x19:
            Sound_PlaySong(0x1cb);
            break;
    }
}
