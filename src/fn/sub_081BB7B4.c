#include "global.h"
void Bg_SetLayerMap(int, void *, int);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
void sub_081BB4E8(void *);
void Sound_PlaySong(int);
void Anim_SetKeyframes(void *, void *, int, int, int);
void sub_08030DBC(int);
void sub_081BBD00(void);
void sub_081BB520(void *, void (*)(void));
void sub_081BB7B4(u8 *p)
{
    int z;
    Bg_SetLayerMap(0, *(void **)(p + 0x6e8), 2);
    z = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, z);
    sub_081BB4E8(p);
    Sound_PlaySong(0x2bf);
    *(u32 *)(p + 0x60) &= ~1;
    Anim_SetKeyframes(p + 0x58, p + 0x18, 2, 2, z);
    sub_08030DBC(3);
    sub_081BB520(p, sub_081BBD00);
}
