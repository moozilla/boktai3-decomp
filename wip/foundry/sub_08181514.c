#define ActorFragments_Launch sub_08181514
#define ActorFragments_Reset sub_08180CE0
#define gFragmentRandomIndex gUnk_03005308
#define gFragmentRandomValues gUnk_0203B400
#include "global.h"
struct ActorFragment {
    u8 pad0[0x24];
    u16 x, y, z;
    u8 pad2A[0xA];
    s16 vx, vy, vz;
    u8 pad3A[4];
    u8 active, pad3F;
};
struct FragmentActor {
    u8 pad0[0xD4];
    u16 x, y, z;
    u8 padDA[0x410-0xDA];
    struct ActorFragment fragments[9];
    u8 pad650[0xB39-0x650];
    u8 direction;
};
struct FragmentOffset { s16 x, y, z, pad; };
extern const struct FragmentOffset gUnk_0824E818[];
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_08180CE0(struct FragmentActor *);
void Sound_PlaySongChecked(u32);
void sub_08181514(struct FragmentActor *p, s32 fast)
{
    s32 group = 0;
    s32 i;
    const struct FragmentOffset *offset;
    switch (((p->direction + 0x60) >> 6) & 3) {
    case 0: group = 1; break;
    case 1: group = 0; break;
    case 2: group = 0; break;
    case 3: group = 1; break;
    }
    sub_08180CE0(p);
    i = 0;
    offset = gUnk_0824E818 + group * 9;
    do {
        struct ActorFragment *f = (struct ActorFragment *)((u8 *)p + 0x410 + i * 64);
        u16 x, z;
        s32 sum;
        f->active = 0;
        x = offset->x;
        f->x = x + p->x;
        f->y = offset->y + p->y;
        z = offset->z;
        sum = z + p->z;
        f->z = sum;
        if (fast) {
            f->vx = (s16)x >> 3;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
            f->vy = gUnk_0203B400[gUnk_03005308] & 0x1F;
            f->vz = (s16)z >> 3;
        } else {
            f->vx = (s16)x >> 5;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
            f->vy = gUnk_0203B400[gUnk_03005308] & 0x1F;
            f->vz = (s16)z >> 5;
        }
        offset++;
        i++;
    } while (i <= 8);
    Sound_PlaySongChecked(0xC4);
}
