#include "global.h"
void sub_082164AC(int, void *, int);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
void sub_081BC664(void *);
void Sound_PlaySong(int);
void Anim_SetKeyframes(void *, void *, int, int, int);
void sub_08030DBC(int);
void sub_081BD084(void);
void sub_081BC694(void *, void (*)(void));
void sub_081BC980(u8 *p)
{
    int z;
    sub_082164AC(0, *(void **)(p + 0x8e0), 5);
    z = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, z);
    sub_081BC664(p);
    Sound_PlaySong(0x2c1);
    *(u32 *)(p + 0x60) &= ~1;
    Anim_SetKeyframes(p + 0x58, p + 0x18, 6, 2, z);
    sub_08030DBC(3);
    sub_081BC694(p, sub_081BD084);
}
