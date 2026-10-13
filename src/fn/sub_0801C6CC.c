// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u16 gUnknown_03005260[];
extern int gUnk_0824DB38[];
extern u8 gUnk_0824DB64[], gUnk_0824DB6F[];
void Sound_PlaySong(int);
void ResultGame_UpdatePlayerMarker(u8 *p)
{
    int *velocity;
    u32 key;
    {
        u32 input = gUnknown_03005260[1];
        register u32 mask asm("r2");
        mask = 1;
        mask &= input;
        asm("" : "+r"(mask));
        key = mask;
    }
    if (key) {
        velocity = (int *)(p + 0x90);
        *velocity = gUnk_0824DB38[*(u32 *)(p + 0x50)];
        Sound_PlaySong(0x3ec);
    } else {
        int v;
        int *initial = (int *)(p + 0x90);
        v = *initial;
        velocity = initial;
        if (v > 0) {
            *velocity = key;
            *((u8 *)initial + 0x22) = key;
        } else {
            u32 index;
            u8 *counter = p + 0xb2;
            register u8 *table asm("r0") = gUnk_0824DB64;
            asm("" : : "r"(counter), "r"(table));
            {
                u32 n;
                n = *(u32 *)(p + 0x50);
                index = n;
            }
            if (p[0xb2] < gUnk_0824DB64[index]) {
                *velocity = key;
                ++p[0xb2];
            } else {
                *velocity = v - gUnk_0824DB6F[index];
                if (*velocity < -22)
                    *velocity = -22;
            }
        }
    }
    *(int *)(p + 0x8c) += *velocity;
}
