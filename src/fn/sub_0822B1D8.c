#include "global.h"
#include "gba/m4a_internal.h"
#include "ram.h"
extern const struct MusicPlayer gUnk_082643EC[];
void m4aMPlayImmInit(struct MusicPlayerInfo *);
void Sound_FadeOutChannel12(u32 speed)
{
    u16 *active = gMusicSlotSongs;
    u32 song = active[12];
    if (song) {
        u32 index = gSongTable[song].ms;
        struct MusicPlayerInfo *player = gUnk_082643EC[index].info;
        m4aMPlayImmInit(player);
        m4aMPlayVolumeControl(player, 0xffff, 255);
        m4aMPlayFadeOut(player, (u16)speed);
        active[index] = 0;
    }
}
