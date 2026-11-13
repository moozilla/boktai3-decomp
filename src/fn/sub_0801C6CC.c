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
    u32 jump = gUnknown_03005260[1] & 1;
    if (jump) {
        p->velocity = gUnk_0824DB38[p->playerIndex];
        Sound_PlaySong(0x3ec);
    } else if (p->velocity > 0) {
        p->velocity = 0;
        p->fallDelay = 0;
    } else if (p->fallDelay < gUnk_0824DB64[p->playerIndex]) {
        p->velocity = 0;
        p->fallDelay++;
    } else {
        p->velocity -= gUnk_0824DB6F[p->playerIndex];
        if (p->velocity < -22)
            p->velocity = -22;
    }
    p->y += p->velocity;
}
