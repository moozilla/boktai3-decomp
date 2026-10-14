#include "global.h"
s32 Mod(s32, s32);
struct Pair081CBC44 {
    u32 a, b;
};
struct Pair081CBC44 *sub_081FEA58(s32);
void sub_081FED40(s32);
void Sound_PlaySong(u32);
void sub_081C8EC0(u8 *);
void sub_081CBC44(u8 *a, u8 *b, s32 frame)
{
    u8 *pos = b + 0x20;
    if (b[9]) {
        u32 zero = 0;
        b[9] = zero;
        b[7] = zero;
        *(struct Pair081CBC44 *)(pos + 8) = *sub_081FEA58(*(s16 *)(pos + 4));
        sub_081FED40(*(s16 *)(pos + 4));
        Sound_PlaySong(0x5ca);
        b[0x18] = zero;
        *(u32 *)(b + 0x10) = zero;
    }
    if (frame <= 30) {
        if (frame <= 19 || Mod(frame, 2) == 0) {
            sub_081C8EC0(b);
            if (frame <= 9) {
                s32 i = 1;
                do {
                    sub_081C8EC0(b);
                    i--;
                } while (i >= 0);
            }
        }
    }
    if (frame > 30)
        b[7] = 1;
}
