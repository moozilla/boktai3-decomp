// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u16 gUnknown_03005260[];
extern int gUnk_0824DB38[];
extern u8 gUnk_0824DB64[], gUnk_0824DB6F[];
void Sound_PlaySong(int);
struct ResultGamePlayerMarker {
    u8 pad00[0x50];
    u32 playerIndex;
    u8 pad54[0x8c - 0x54];
    s32 y;
    s32 velocity;
    u8 pad94[0xb2 - 0x94];
    u8 fallDelay;
};
void ResultGame_UpdatePlayerMarker(struct ResultGamePlayerMarker *p)
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
        velocity = &p->velocity;
        *velocity = gUnk_0824DB38[p->playerIndex];
        Sound_PlaySong(0x3ec);
    } else {
        int *initial = &p->velocity;
        int v = *initial;
        velocity = initial;
        if (v > 0) {
            *velocity = key;
            p->fallDelay = key;
        } else {
            u8 *counter = &p->fallDelay;
            u8 *table = gUnk_0824DB64;
            u32 index = p->playerIndex;
            if (*counter < table[index]) {
                *velocity = key;
                ++*counter;
            } else {
                *velocity = v - gUnk_0824DB6F[index];
                if (*velocity < -22)
                    *velocity = -22;
            }
        }
    }
    p->y += *velocity;
}
