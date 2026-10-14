#include "global.h"
#include "game_state.h"

struct Vec08196718 {
    s16 x, y, z, w;
};
struct P08196718 {
    u8 f0[0x54];
    u8 effects[0x5404 - 0x54];
    struct Vec08196718 pos;
    struct Vec08196718 base;
    s16 speed;
    u8 f1[0x7244 - 0x5416];
    s16 timer;
    u8 f2[0x7249 - 0x7246];
    u8 mode;
    u8 state;
    u8 f3[0x72a8 - 0x724b];
    s32 a, b, c, d, e, f;
};
extern const struct Vec08196718 gUnk_0824F598, gUnk_0824F5A8, gUnk_0824F5B0, gUnk_0824F5B8;
extern const u8 gUnk_0824F5A0[];
void sub_08193680(void *, u32);
void sub_08195014(void *, s32, s32);
void sub_08195180(void *);
void sub_08195254(void *);
void sub_08190498(void *, void *, const void *, void *);
void sub_08195A4C(void *, u32);
void Sound_PlaySongChecked(u32);
void Sound_StopSong(u32);

void sub_08196718(struct P08196718 *t)
{
    switch (t->state) {
        case 0:
            t->base.x = -0xe80;
            t->base.y = t->pos.y;
            t->base.z = t->pos.z;
            sub_08195014(t, t->base.x, t->base.z);
            t->d = -0xe00;
            t->e = -0x88;
            t->state++;
            t->timer = 0;
            break;
        case 1:
            if (t->timer == 6)
                Sound_PlaySongChecked(0x409);
            t->a += t->d;
            if (t->a < 0)
                t->state++;
            goto update;
        case 2:
            sub_08193680(t, 1);
            t->base = gUnk_0824F598;
            t->base.z += gUnk_02000710->positionZ;
            t->speed = 0;
            t->pos = t->base;
            t->pos.x += -0x800;
            sub_08195014(t, t->base.x, t->base.z);
            t->d = 0xe00;
            t->state++;
            Sound_PlaySongChecked(0x405);
            break;
        case 3:
            t->a += t->d;
            if (t->a > 0) {
                t->state++;
                t->timer = 0;
                t->a = 0;
                t->d = 0;
            }
            goto update;
        case 4:
            if (t->timer > 0x28) {
                t->state++;
                t->timer = 0;
                Sound_PlaySongChecked(0x1b8);
            }
            goto update;
        case 5:
            if (!(t->timer & 1))
                sub_08190498(t->effects, &t->pos, gUnk_0824F5A0, t);
            sub_08195254(t);
            sub_08195180(t);
            if (t->timer > 0x78) {
                t->state++;
                Sound_StopSong(0x1b8);
                Sound_PlaySongChecked(0x40a);
            }
            break;
        case 6:
            t->a += t->d;
            if (t->a < -0x80000)
                t->state++;
            t->d -= 6;
            if (t->d < -0xe00)
                t->d = -0xe00;
        update:
            sub_08195180(t);
            break;
        case 7:
            sub_08193680(t, 0);
            switch (t->mode) {
                case 8:
                    t->pos = gUnk_0824F5A8;
                    sub_08195A4C(t, 2);
                    break;
                case 9:
                    t->pos = gUnk_0824F5B0;
                    sub_08195A4C(t, 4);
                    break;
                case 10:
                    t->pos = gUnk_0824F5B8;
                    sub_08195A4C(t, 0xb);
                    break;
            }
            t->d = 0;
            break;
    }
}
