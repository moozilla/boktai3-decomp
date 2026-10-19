#include "global.h"

extern u8 *gUnk_020001AC;
extern u8 *gUnk_02000580[4];

struct Follow {
    u8 pad0[0x40];
    void *follow;
    u8 pad44[0x284 - 0x44];
    u8 sub[1];
};

void sub_0810103C(u8 *e)
{
    s32 i;
    u32 *managerPtr = (u32 *)(gUnk_020001AC + 0x1370);
    struct Follow **pp;
    struct Follow *player;

    i = 0;
    pp = (struct Follow **)gUnk_02000580;
    do {
        player = *pp;
        if (player != NULL) {
            if (*managerPtr == *(u32 *)(e + 0xDC)) {
                if ((u16)(*(u16 *)(e + 0xB6) - 3) <= 1) {
                    player->follow = player->sub;
                } else {
                    player->follow = 0;
                }
            }
        }
        pp++;
    } while (--i >= 0);
}
