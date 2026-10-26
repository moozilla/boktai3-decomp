#include "global.h"
#include "ram.h"
struct SegmentDisplay;
void Sprite_Remove(void *);
void Anim_Remove(void *);
void Sound_StopSong(u32);
s32 SegmentDisplay_Destroy(struct SegmentDisplay *p)
{
    s32 i;
    u8 *anim;
    u8 *segments;
    u8 *markers;
    Sprite_Remove((u8 *)p + 0x20);
    Sprite_Remove((u8 *)p + 0x6c);
    Sprite_Remove((u8 *)p + 0xb8);
    anim = (void *)((u8 *)p + 0x398);
    i = 4;
    do {
        Anim_Remove(anim);
        anim = (void *)((u8 *)anim + 0x60);
        i--;
    } while (i >= 0);
    segments = (void *)((u8 *)p + 0x100);
    i = 6;
    do {
        Sprite_Remove(segments);
        segments += 0x2c;
        i--;
    } while (i >= 0);
    markers = (void *)((u8 *)p + 0x250);
    i = 5;
    do {
        Sprite_Remove(markers);
        markers += 0x2c;
        i--;
    } while (i >= 0);
    {
        u32 zero = 0;
        gSegmentDisplay = (void *)zero;
    }
    Sound_StopSong(0x23e);
    return 0;
}
