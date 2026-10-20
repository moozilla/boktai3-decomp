#include "global.h"
struct Sprite {
    u8 head[8];
    u32 flags;
    u8 gap[0x1b - 12];
    u8 mode;
    u8 gap2[4];
    s16 x, y;
    u8 tail[0x60 - 0x24];
};
struct Viewer {
    u8 p0[0x240];
    u32 digitsResource[8];
    u8 p1[0x4a0 - 0x260];
    struct Sprite labels[3];
    u8 p5[0x5ec - 0x5c0];
    s32 previousA, previousB, previousC;
};
extern const s16 gUnk_086136F0[], gUnk_086137AE[], gUnk_08613BC4[];
u32 Sound_IsSongPlaying(s32);
s32 Anim_Step(void *, void *);
// Advance each category's indicator animation while its song plays.
void SoundTest_UpdatePlaybackIndicators(struct Viewer *p)
{
    s32 i;
    for (i = 0; i <= 2; i++) {
        const s16 *table;
        s32 index;
        struct Sprite *s = &p->labels[i];
        switch (i) {
            case 0:
                table = gUnk_086136F0;
                index = p->previousA;
                break;
            case 1:
                table = gUnk_086137AE;
                index = p->previousB;
                break;
            case 2:
                table = gUnk_08613BC4;
                index = p->previousC;
                break;
            default:
                return;
        }
        if (Sound_IsSongPlaying(table[index])) {
            s->flags &= ~1;
            Anim_Step(s, p->digitsResource);
        } else if (!(s->flags & 1)) {
            s->mode = 2;
            if (Anim_Step(s, p->digitsResource))
                s->flags |= 1;
        }
    }
}
