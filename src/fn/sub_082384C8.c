#define gPlayerLinkSelection gUnk_020004B4
#include "global.h"
#include "player.h"
struct PlayerLinkSelection {
    u8 unknown0[0x18];
    s16 localPlayer;
};
extern struct PlayerLinkSelection *gUnk_020004B4;
void Sound_StopSong(u32);
static inline s32 LocalPlayerSlot(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}
void Player_StopLocalActionSounds(struct Player *p)
{
    if (p->slot == LocalPlayerSlot()) {
        if (p->actionMode != 6) {
            Sound_StopSong(0xd8);
        } else {
            Sound_StopSong(0x239);
            Sound_StopSong(0x202);
            Sound_StopSong(0x366);
        }
    }
}
