#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.8%
// 3 differing instruction runs; first diff at +0x2 (adds r7, r0, #0 vs adds r6, r0, #0)
#include "gba/m4a_internal.h"
extern u16 gUnk_03005470[];
void m4aSongNumStartOrContinue(u16);
void m4aSongNumStart(u16);
void m4aSongNumStop(u16);
void m4aMPlayImmInit(struct MusicPlayerInfo *);
void m4aMPlayFadeIn(struct MusicPlayerInfo *, u16);
void m4aMPlayFadeOut(struct MusicPlayerInfo *, u16);
void m4aMPlayFadeOutTemporarily(struct MusicPlayerInfo *, u16);
void sub_0822AFE8(u32 arg)
{
    u32 speed = arg;
    u16 *slots = gUnk_03005470;
    u32 song = slots[10];
    if (song) {
        u32 index = gSongTable[song].ms;
        struct MusicPlayerInfo *player = gMPlayTable[index].info;
        m4aMPlayImmInit(player);
        m4aMPlayVolumeControl(player, 0xffff, 256);
        m4aMPlayFadeOutTemporarily(player, speed);
        slots[index] = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0822AFE8);
#endif
